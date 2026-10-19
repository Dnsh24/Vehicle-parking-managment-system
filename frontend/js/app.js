const API_BASE = "http://localhost:3000";
const selectedSlot = document.getElementById("selectedSlot");
const selectedStatus = document.getElementById("selectedStatus");
const selectedVisual = document.getElementById("selectedVisual");
const detailStatus = document.getElementById("detailStatus");
const detailVehicle = document.getElementById("detailVehicle");
const detailType = document.getElementById("detailType");
const detailFeatures = document.getElementById("detailFeatures");
const parkingMap = document.getElementById("parkingMap");
const floorTitle = document.getElementById("floorTitle");
const availableCount = document.getElementById("availableCount");
const areaName = document.querySelector(".area-picker h2");
const floorTabs = document.querySelectorAll(".floor-tab");

let areas = [];
let selectedArea = null;

function statusLabel(status) {
    return status === "available" ? "Available" : status[0].toUpperCase() + status.slice(1);
}

function displayVehicleType(vehicleType, isEv) {
    if (isEv) {
        return "EV car";
    }
    return vehicleType[0].toUpperCase() + vehicleType.slice(1);
}

function imageNameFor(vehicleType, isEv) {
    if (isEv) {
        return "ev-car-top-view.svg";
    }
    if (vehicleType === "car") {
        return "car-top-view.svg";
    }
    return `${vehicleType}-top-view.svg`;
}

function sectionForSlot(slot) {
    const slotType = slot.slot_type.toLowerCase();
    if (slot.vehicle_type === "truck") {
        return "truck";
    }
    if (slot.vehicle_type === "bike" || slot.vehicle_type === "scooter") {
        return "bike";
    }
    if (slot.is_ev) {
        return "ev-car";
    }
    if (slotType.includes("handicap") || slotType.includes("accessible")) {
        return "accessible-car";
    }
    return "car";
}

function renderMessage(title, message, action = "") {
    parkingMap.innerHTML = `<div class="map-message"><strong>${title}</strong><span>${message}</span>${action}</div>`;
}

function renderSelectedVisual(slot, statusClass, vehicleType) {
    const vehicleImage = slot.classList.contains("occupied")
        ? `<img class="selected-vehicle-image ${vehicleType}" src="assets/images/${imageNameFor(vehicleType, slot.classList.contains("ev"))}" alt="${displayVehicleType(vehicleType, slot.classList.contains("ev"))}">`
        : "";
    const marker = vehicleImage || "<span aria-hidden=\"true\">♢</span>";

    selectedVisual.className = `selected-slot-visual ${statusClass}`;
    selectedVisual.innerHTML = `${marker}<strong>${slot.dataset.slot}</strong>`;
}

function selectSlot(slot) {
    const [vehicle, type, status] = slot.dataset.info.split(" · ");
    const statusClass = slot.classList.contains("available") ? "available" : slot.classList[1];
    selectedSlot.textContent = slot.dataset.slot;
    selectedStatus.textContent = status;
    selectedStatus.className = `status-pill ${statusClass}`;
    renderSelectedVisual(slot, statusClass, slot.dataset.vehicle);
    detailStatus.textContent = status === "Available" ? "Available to use" : status;
    detailVehicle.textContent = vehicle;
    detailType.textContent = type;
    detailFeatures.innerHTML = slot.classList.contains("ev")
        ? '<span class="feature-tag">⚡ EV charging</span>'
        : slot.classList.contains("accessible")
            ? '<span class="feature-tag">♿ Accessible</span>'
            : '<span class="muted">Standard</span>';
}

function renderFloor(area, slots) {
    const groupedSlots = {
        truck: [],
        bike: [],
        car: [],
        "ev-car": [],
        "accessible-car": []
    };

    slots.forEach((slot) => groupedSlots[sectionForSlot(slot)].push(slot));
    const sections = [
        ["truck", "Truck / oversized"],
        ["bike", "Bike & scooter parking"],
        ["car", "Normal car parking"],
        ["ev-car", "EV charging"],
        ["accessible-car", "Handicapped parking"]
    ];

    floorTitle.textContent = area.name;
    areaName.textContent = area.name;
    availableCount.innerHTML = `${area.available_slots} <small>of ${area.capacity} slots</small>`;
    parkingMap.innerHTML = `<div class="map-edge entrance"><span>↓</span> ENTRANCE / RAMP</div>
        ${sections.map(([sectionClass, sectionTitle]) => `<section class="category-section ${sectionClass}">
            <div class="category-title">${sectionTitle}<span>${groupedSlots[sectionClass].length} slots</span></div>
            <div class="map-row">${groupedSlots[sectionClass].map((slot) => {
            const label = statusLabel(slot.status);
            const vehicleType = displayVehicleType(slot.vehicle_type, slot.is_ev);
            const slotType = slot.slot_type;
            const classes = `${slot.status}${slot.is_ev ? " ev" : ""}${sectionClass === "accessible-car" ? " accessible" : ""}`;
            const vehicleImage = slot.status === "occupied"
                ? `<img class="vehicle-image ${slot.vehicle_type}" src="assets/images/${imageNameFor(slot.vehicle_type, slot.is_ev)}" alt="" aria-hidden="true">`
                : "";
            const accessibility = sectionClass === "accessible-car" ? "♿ " : "";
            const ev = slot.is_ev ? "EV · " : "";
            return `<div class="slot ${classes}" data-slot="${slot.slot_number}" data-vehicle="${slot.vehicle_type}" data-info="${vehicleType} · ${slotType} · ${label}" tabindex="0">${vehicleImage}<b>${slot.slot_number}</b><small>${accessibility}${ev}${label}</small></div>`;
        }).join("")}</div></section><div class="lane"><span>←</span> DRIVE LANE <span>→</span></div>`).join("")}
        <div class="map-edge exit">EXIT / RAMP <span>↓</span></div>`;

    parkingMap.querySelectorAll(".slot").forEach((slot) => {
        slot.addEventListener("click", () => selectSlot(slot));
        slot.addEventListener("keydown", (event) => {
            if (event.key === "Enter" || event.key === " ") {
                event.preventDefault();
                selectSlot(slot);
            }
        });
    });
}

async function loadFloor(area) {
    selectedArea = area;
    floorTitle.textContent = `Loading ${area.name}`;
    renderMessage("Loading parking layout", "Fetching the latest slot availability...");

    try {
        const response = await fetch(`${API_BASE}/api/parking-areas/${area.area_id}/slots`);
        const data = await response.json();
        if (!response.ok || !Array.isArray(data.slots)) {
            throw new Error(data.error?.message || "Parking slots could not be loaded.");
        }
        if (data.slots.length === 0) {
            renderMessage("No slots configured", "This parking floor does not have any configured slots.");
            return;
        }
        renderFloor(area, data.slots);
    } catch (error) {
        renderMessage("Parking layout unavailable", error.message, '<button class="map-retry" type="button">Try again</button>');
        parkingMap.querySelector(".map-retry").addEventListener("click", () => loadFloor(area));
    }
}

function setFloorTab(area, index) {
    const tab = floorTabs[index];
    if (!tab) {
        return;
    }
    tab.hidden = false;
    tab.dataset.areaId = area.area_id;
    tab.textContent = area.name.replace("Mall Central — ", "").replace("Mall Central - ", "");
    const badge = document.createElement("span");
    badge.textContent = index === 0 ? "G" : String(index);
    tab.appendChild(badge);
    tab.classList.toggle("active", index === 0);
    tab.addEventListener("click", () => {
        floorTabs.forEach((item) => item.classList.remove("active"));
        tab.classList.add("active");
        loadFloor(area);
    });
}

async function loadAreas() {
    renderMessage("Loading parking floors", "Fetching Mall Central parking areas...");
    try {
        const response = await fetch(`${API_BASE}/api/parking-areas`);
        const data = await response.json();
        if (!response.ok || !Array.isArray(data.areas)) {
            throw new Error(data.error?.message || "Parking areas could not be loaded.");
        }
        areas = data.areas;
        if (areas.length === 0) {
            renderMessage("No parking floors available", "Mall Central has no active parking areas configured.");
            return;
        }
        floorTabs.forEach((tab, index) => {
            tab.hidden = index >= areas.length;
            if (areas[index]) {
                setFloorTab(areas[index], index);
            }
        });
        await loadFloor(areas[0]);
    } catch (error) {
        renderMessage("Parking information unavailable", error.message, '<button class="map-retry" type="button">Try again</button>');
        parkingMap.querySelector(".map-retry").addEventListener("click", loadAreas);
    }
}

document.getElementById("refreshButton").addEventListener("click", (event) => {
    const button = event.currentTarget;
    button.innerHTML = '<span aria-hidden="true">↻</span> Updated';
    if (selectedArea) {
        loadFloor(selectedArea);
    }
    window.setTimeout(() => {
        button.innerHTML = '<span aria-hidden="true">↻</span> Refresh';
    }, 1200);
});

document.getElementById("changeAreaButton").addEventListener("click", () => {
    window.alert("Mall Central floors are available in the floor selector.");
});

loadAreas();

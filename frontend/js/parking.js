// =====================================================================
// ParkFlow - Parking Map & Area Management Module
// =====================================================================

const selectedSlot = document.getElementById("selectedSlot");
const selectedStatus = document.getElementById("selectedStatus");
const selectedVisual = document.getElementById("selectedVisual");
const detailStatus = document.getElementById("detailStatus");
const detailVehicle = document.getElementById("detailVehicle");
const detailType = document.getElementById("detailType");
const detailFeatures = document.getElementById("detailFeatures");
const detailRegistrationRow = document.getElementById("detailRegistrationRow");
const detailRegistration = document.getElementById("detailRegistration");
const detailDurationRow = document.getElementById("detailDurationRow");
const detailDuration = document.getElementById("detailDuration");
const parkingMap = document.getElementById("parkingMap");
const floorTitle = document.getElementById("floorTitle");
const availableCount = document.getElementById("availableCount");
const areaName = document.querySelector(".area-picker h2");
const floorTabsNav = document.getElementById("floorTabsNav") || document.querySelector(".floor-tabs");
const areaDropdown = document.getElementById("areaDropdown");
const areaPickerTitle = document.getElementById("areaPickerTitle");
const summaryHours = document.getElementById("summaryHours");
const summaryLocation = document.getElementById("summaryLocation");
const refreshButton = document.getElementById("refreshButton") || document.getElementById("refreshMapButton");
const changeAreaButton = document.getElementById("changeAreaButton");

let areas = [];
let selectedArea = null;
let selectedParkedSeconds = 0;
let selectedDurationTimer = null;
let slotSelection = null;

function renderMessage(title, message, action = "") {
    if (parkingMap) {
        parkingMap.innerHTML = `<div class="map-message"><strong>${title}</strong><span>${message}</span>${action}</div>`;
    }
}

function renderSelectedVisual(slot, statusClass, vehicleType) {
    if (!selectedVisual) return;
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
    if (selectedSlot) selectedSlot.textContent = slot.dataset.slot;
    if (selectedStatus) {
        selectedStatus.textContent = status;
        selectedStatus.className = `status-pill ${statusClass}`;
    }
    renderSelectedVisual(slot, statusClass, slot.dataset.vehicle);
    if (detailStatus) detailStatus.textContent = status === "Available" ? "Available to use" : status;
    const actualVehicleType = slot.dataset.vehicleRegistration
        ? slot.dataset.vehicle
        : null;
    if (detailVehicle) {
        detailVehicle.textContent = actualVehicleType
            ? displayVehicleType(actualVehicleType, slot.dataset.vehicleElectric === "true")
            : vehicle;
    }
    if (detailType) detailType.textContent = type;
    const registration = slot.dataset.vehicleRegistration || "";
    if (detailRegistrationRow) detailRegistrationRow.hidden = !registration;
    if (detailRegistration) detailRegistration.textContent = registration;
    if (detailDurationRow) detailDurationRow.hidden = !registration || !slot.dataset.parkedAt;
    if (selectedDurationTimer !== null) {
        window.clearInterval(selectedDurationTimer);
        selectedDurationTimer = null;
    }
    selectedParkedSeconds = Number(slot.dataset.parkedSeconds || 0);
    if (registration && slot.dataset.parkedAt) {
        const startedAt = Date.parse(slot.dataset.parkedAt);
        const updateDuration = () => {
            const liveSeconds = Number.isNaN(startedAt)
                ? selectedParkedSeconds
                : Math.floor((Date.now() - startedAt) / 1000);
            if (detailDuration) {
                detailDuration.textContent = formatDuration(Math.max(selectedParkedSeconds, liveSeconds));
            }
        };
        updateDuration();
        selectedDurationTimer = window.setInterval(updateDuration, 30000);
    }
    if (detailFeatures) {
        detailFeatures.innerHTML = slot.classList.contains("ev")
            ? '<span class="feature-tag">⚡ EV charging</span>'
            : slot.classList.contains("accessible")
                ? '<span class="feature-tag">♿ Accessible</span>'
                : '<span class="muted">Standard</span>';
    }
}

function applySlotSelection() {
    if (!parkingMap) return;
    parkingMap.querySelectorAll(".slot").forEach((slot) => {
        const selectable = slotSelection &&
            slotSelection.slotIds.has(String(slot.dataset.slotId));
        if (selectable) {
            slot.dataset.selectVehicleId = slotSelection.vehicleId;
            slot.classList.add("slot-selectable");
            slot.title = "Choose this available slot";
        } else {
            delete slot.dataset.selectVehicleId;
            slot.classList.remove("slot-selectable");
        }
    });
}

function renderFloor(area, slots) {
    if (!parkingMap) return;
    const groupedSlots = {
        truck: [],
        bike: [],
        car: [],
        "ev-car": [],
        "accessible-car": []
    };

    slots.forEach((slot) => {
        const section = sectionForSlot(slot);
        if (groupedSlots[section]) {
            groupedSlots[section].push(slot);
        }
    });

    const sections = [
        ["truck", "Truck / oversized"],
        ["bike", "Bike & scooter parking"],
        ["car", "Normal car parking"],
        ["ev-car", "EV charging"],
        ["accessible-car", "Handicapped parking"]
    ];

    if (floorTitle) floorTitle.textContent = area.name;
    if (areaName) areaName.textContent = area.name;
    const availableSlots = slots.filter((slot) => slot.status === "available").length;
    if (availableCount) {
        availableCount.innerHTML = `${availableSlots} <small>of ${area.capacity} slots</small>`;
    }
    parkingMap.innerHTML = `<div class="map-edge entrance"><span>↓</span> ENTRANCE / RAMP</div>
        ${sections.map(([sectionClass, sectionTitle]) => `<section class="category-section ${sectionClass}">
            <div class="category-title">${sectionTitle}<span>${groupedSlots[sectionClass].length} slots</span></div>
            <div class="map-row">${groupedSlots[sectionClass].map((slot) => {
            const label = statusLabel(slot.status);
            const vehicleType = displayVehicleType(slot.vehicle_type, slot.is_ev);
            const slotType = slot.slot_type;
            const classes = `${slot.status}${slot.is_ev ? " ev" : ""}${sectionClass === "accessible-car" ? " accessible" : ""}`;
            const parked = Boolean(slot.vehicle_registration);
            const visibleVehicleType = parked ? slot.parked_vehicle_type : slot.vehicle_type;
            const vehicleImage = parked
                ? `<img class="vehicle-image ${visibleVehicleType}" src="assets/images/${imageNameFor(visibleVehicleType, slot.is_ev)}" alt="" aria-hidden="true">`
                : "";
            const accessibility = sectionClass === "accessible-car" ? "♿ " : "";
            const ev = slot.is_ev ? "EV · " : "";
            const slotContents = parked
                ? `<b>${slot.slot_number}</b><small class="slot-registration"></small>`
                : `<b>${slot.slot_number}</b><small>${accessibility}${ev}${label}</small>`;
            return `<div class="slot ${classes}" data-slot-id="${slot.slot_id}" data-slot="${slot.slot_number}" data-vehicle="${slot.vehicle_type}" data-info="${vehicleType} · ${slotType} · ${label}" tabindex="0">${vehicleImage}${slotContents}</div>`;
        }).join("")}</div></section><div class="lane"><span>←</span> DRIVE LANE <span>→</span></div>`).join("")}
        <div class="map-edge exit">EXIT / RAMP <span>↓</span></div>`;

    slots.forEach((slot) => {
        const element = parkingMap.querySelector(`[data-slot-id="${CSS.escape(String(slot.slot_id))}"]`);
        if (!element) {
            return;
        }
        if (slot.vehicle_registration) {
            element.dataset.vehicleRegistration = slot.vehicle_registration;
            element.dataset.parkedAt = slot.parked_at || "";
            element.dataset.parkedSeconds = String(slot.parked_seconds || 0);
            element.dataset.vehicle = slot.parked_vehicle_type || slot.vehicle_type;
            element.dataset.vehicleElectric = String(slot.parked_vehicle_is_electric);
            element.title = `${slot.vehicle_registration} · ${slot.slot_type}`;
            const regEl = element.querySelector(".slot-registration");
            if (regEl) regEl.textContent = slot.vehicle_registration;
        } else {
            element.title = `${slot.slot_number} · ${statusLabel(slot.status)}`;
        }
    });
    applySlotSelection();

    parkingMap.querySelectorAll(".slot").forEach((slot) => {
        slot.addEventListener("click", () => {
            if (slot.getAttribute("aria-disabled") === "true") {
                return;
            }
            if (slot.dataset.selectVehicleId) {
                if (typeof parkVehicleInSlot === "function") {
                    parkVehicleInSlot(slot.dataset.selectVehicleId, slot.dataset.slotId);
                }
                return;
            }
            selectSlot(slot);
        });
        slot.addEventListener("keydown", (event) => {
            if (event.key === "Enter" || event.key === " ") {
                event.preventDefault();
                if (slot.getAttribute("aria-disabled") === "true") {
                    return;
                }
                if (slot.dataset.selectVehicleId) {
                    if (typeof parkVehicleInSlot === "function") {
                        parkVehicleInSlot(slot.dataset.selectVehicleId, slot.dataset.slotId);
                    }
                    return;
                }
                selectSlot(slot);
            }
        });
    });
}

function selectParkingArea(area) {
    if (!area) return;
    selectedArea = area;

    const titleEl = areaPickerTitle || areaName;
    if (titleEl) titleEl.textContent = area.name;

    if (summaryHours) {
        summaryHours.textContent = area.operating_hours || "07:00 – 22:00";
    }

    if (summaryLocation) {
        summaryLocation.innerHTML = `${escapeHtml(area.location || "Mall Central")} <small>Parking building</small>`;
    }

    if (floorTabsNav) {
        floorTabsNav.querySelectorAll(".floor-tab").forEach((tab) => {
            tab.classList.toggle("active", String(tab.dataset.areaId) === String(area.area_id));
        });
    }

    if (areaDropdown) {
        areaDropdown.querySelectorAll(".area-dropdown-item").forEach((item) => {
            item.classList.toggle("active", String(item.dataset.areaId) === String(area.area_id));
        });
        areaDropdown.hidden = true;
        if (changeAreaButton) changeAreaButton.setAttribute("aria-expanded", "false");
    }

    loadFloor(area);
}

function renderFloorTabs() {
    if (!floorTabsNav) return;
    floorTabsNav.replaceChildren();

    areas.forEach((area, index) => {
        const tab = document.createElement("button");
        const isActive = selectedArea ? String(selectedArea.area_id) === String(area.area_id) : index === 0;
        tab.className = "floor-tab" + (isActive ? " active" : "");
        tab.type = "button";
        tab.dataset.areaId = area.area_id;

        const shortName = area.name.replace("Mall Central — ", "").replace("Mall Central - ", "");
        tab.textContent = shortName;

        const badge = document.createElement("span");
        const lowerName = shortName.toLowerCase();
        if (lowerName.includes("ground") || shortName === "G") {
            badge.textContent = "G";
        } else if (lowerName.includes("first") || shortName === "1") {
            badge.textContent = "1";
        } else if (lowerName.includes("second") || shortName === "2") {
            badge.textContent = "2";
        } else if (lowerName.includes("third") || shortName === "3") {
            badge.textContent = "3";
        } else if (/^\d+$/.test(shortName)) {
            badge.textContent = shortName;
        } else {
            badge.textContent = shortName.charAt(0).toUpperCase();
        }
        tab.appendChild(badge);

        tab.addEventListener("click", () => {
            selectParkingArea(area);
        });

        floorTabsNav.appendChild(tab);
    });
}

function renderAreaDropdown() {
    if (!areaDropdown) return;

    areaDropdown.innerHTML = areas.map((area) => {
        const isActive = selectedArea && String(selectedArea.area_id) === String(area.area_id);
        const availCount = typeof area.available_slots === "number" ? `${area.available_slots} available` : `${area.capacity} slots`;
        return `
            <li class="area-dropdown-item${isActive ? " active" : ""}" role="option" data-area-id="${area.area_id}">
                <div class="area-dropdown-item-header">
                    <strong>${escapeHtml(area.name)}</strong>
                    <span class="area-dropdown-badge">${availCount}</span>
                </div>
                <div class="area-dropdown-item-sub">${escapeHtml(area.location || "Mall Central")} · ${escapeHtml(area.operating_hours || "07:00 – 22:00")}</div>
            </li>
        `;
    }).join("");

    areaDropdown.querySelectorAll(".area-dropdown-item").forEach((item) => {
        item.addEventListener("click", () => {
            const areaId = item.dataset.areaId;
            const targetArea = areas.find((a) => String(a.area_id) === String(areaId));
            if (targetArea) {
                selectParkingArea(targetArea);
            }
        });
    });
}

async function loadFloor(area) {
    selectedArea = area;
    if (floorTitle) floorTitle.textContent = `Loading ${area.name}`;
    renderMessage("Loading parking layout", "Fetching the latest slot availability...");

    try {
        const response = await fetch(`${API_BASE}/api/parking-areas/${area.area_id}/slots`);
        const data = await response.json();
        if (!response.ok || !Array.isArray(data.slots)) {
            throw new Error(data.error?.message || "Parking slots could not be loaded.");
        }
        if (data.slots.length === 0) {
            if (floorTitle) floorTitle.textContent = area.name;
            if (areaName) areaName.textContent = area.name;
            if (areaPickerTitle) areaPickerTitle.textContent = area.name;
            if (availableCount) availableCount.innerHTML = `0 <small>of ${area.capacity} slots</small>`;
            renderMessage("No slots configured", `Parking area "${area.name}" has no slots configured yet.`);
            return;
        }
        renderFloor(area, data.slots);
    } catch (error) {
        renderMessage("Parking layout unavailable", error.message, '<button class="map-retry" type="button">Try again</button>');
        const retryBtn = parkingMap ? parkingMap.querySelector(".map-retry") : null;
        if (retryBtn) {
            retryBtn.addEventListener("click", () => loadFloor(area));
        }
    }
}

async function loadAreas() {
    renderMessage("Loading parking floors", "Fetching parking areas...");
    try {
        const response = await fetch(`${API_BASE}/api/parking-areas`);
        const data = await response.json();
        if (!response.ok || !Array.isArray(data.areas)) {
            throw new Error(data.error?.message || "Parking areas could not be loaded.");
        }
        areas = data.areas;
        if (areas.length === 0) {
            renderMessage("No parking floors available", "No active parking areas configured.");
            return;
        }

        renderFloorTabs();
        renderAreaDropdown();

        const current = selectedArea ? areas.find((a) => String(a.area_id) === String(selectedArea.area_id)) : null;
        selectParkingArea(current || areas[0]);
    } catch (error) {
        renderMessage("Parking information unavailable", error.message, '<button class="map-retry" type="button">Try again</button>');
        const retryBtn = parkingMap ? parkingMap.querySelector(".map-retry") : null;
        if (retryBtn) {
            retryBtn.addEventListener("click", loadAreas);
        }
    }
}

if (refreshButton) {
    refreshButton.addEventListener("click", (event) => {
        const button = event.currentTarget;
        button.innerHTML = '<span aria-hidden="true">↻</span> Updated';
        if (selectedArea) {
            loadFloor(selectedArea);
        }
        window.setTimeout(() => {
            button.innerHTML = '<span aria-hidden="true">↻</span> Refresh';
        }, 1200);
    });
}

if (changeAreaButton) {
    changeAreaButton.addEventListener("click", (event) => {
        event.stopPropagation();
        if (!areaDropdown) return;
        const isHidden = areaDropdown.hidden;
        areaDropdown.hidden = !isHidden;
        changeAreaButton.setAttribute("aria-expanded", String(isHidden));
    });
}

document.addEventListener("click", (event) => {
    if (areaDropdown && !areaDropdown.hidden && !event.target.closest("#areaDropdownWrapper")) {
        areaDropdown.hidden = true;
        if (changeAreaButton) changeAreaButton.setAttribute("aria-expanded", "false");
    }
});

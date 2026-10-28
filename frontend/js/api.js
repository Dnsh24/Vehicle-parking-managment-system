// =====================================================================
// ParkFlow - Shared API & Helper Utilities
// =====================================================================

const API_BASE = "http://localhost:3000";
let appNoticeTimeout = null;

function showAppNotice(message, type = "success") {
    const notice = document.getElementById("appNotice");
    const noticeMessage = document.getElementById("appNoticeMessage");
    if (!notice || !noticeMessage || !message) return;

    noticeMessage.textContent = message;
    notice.classList.toggle("is-error", type === "error");
    notice.classList.toggle("is-info", type === "info");
    notice.setAttribute("role", type === "error" ? "alert" : "status");
    notice.setAttribute("aria-live", type === "error" ? "assertive" : "polite");
    notice.hidden = false;
    window.clearTimeout(appNoticeTimeout);
    appNoticeTimeout = window.setTimeout(() => {
        notice.hidden = true;
    }, 6500);
}

document.getElementById("appNoticeClose")?.addEventListener("click", () => {
    const notice = document.getElementById("appNotice");
    if (notice) notice.hidden = true;
    window.clearTimeout(appNoticeTimeout);
});

function showAppConfirm(message, options = {}) {
    const dialog = document.getElementById("appConfirmDialog");
    const title = document.getElementById("appConfirmTitle");
    const content = document.getElementById("appConfirmMessage");
    const cancelButton = document.getElementById("appConfirmCancel");
    const acceptButton = document.getElementById("appConfirmAccept");
    if (!dialog || !title || !content || !cancelButton || !acceptButton) {
        console.error("The in-app confirmation dialog is unavailable.");
        return Promise.resolve(false);
    }

    title.textContent = options.title || "Confirm action";
    content.textContent = message;
    cancelButton.textContent = options.cancelLabel || "Cancel";
    acceptButton.textContent = options.acceptLabel || "Continue";

    return new Promise((resolve) => {
        const handleClose = () => {
            cleanup();
            resolve(dialog.returnValue === "confirm");
        };
        const handleCancel = (event) => {
            event.preventDefault();
            dialog.close("cancel");
        };
        const cleanup = () => {
            dialog.removeEventListener("cancel", handleCancel);
            cancelButton.removeEventListener("click", handleCancelClick);
            acceptButton.removeEventListener("click", handleAcceptClick);
        };
        const finish = (value) => {
            dialog.close(value);
        };
        const handleCancelClick = () => finish("cancel");
        const handleAcceptClick = () => finish("confirm");

        dialog.addEventListener("close", handleClose, { once: true });
        dialog.addEventListener("cancel", handleCancel);
        cancelButton.addEventListener("click", handleCancelClick);
        acceptButton.addEventListener("click", handleAcceptClick);
        dialog.showModal();
        acceptButton.focus();
    });
}

function authHeaders() {
    const token = window.localStorage.getItem("parking_auth_token") || window.sessionStorage.getItem("parking_auth_token");
    return token ? { Authorization: `Bearer ${token}` } : {};
}

function escapeHtml(value) {
    if (value === null || value === undefined) return "";
    return String(value)
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#39;");
}

function statusLabel(status) {
    return status === "available" ? "Available" : status[0].toUpperCase() + status.slice(1);
}

function formatDuration(totalSeconds) {
    const seconds = Math.max(0, Math.floor(totalSeconds));
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    if (days > 0) {
        return `${days}d ${hours}h ${minutes}m`;
    }
    if (hours > 0) {
        return `${hours}h ${minutes}m`;
    }
    return `${minutes}m`;
}

function parkingFeeBreakdown(summary) {
    const hasGraceBuffer = Number.isFinite(Number(summary.grace_buffer_minutes));
    const graceBufferMinutes = hasGraceBuffer
        ? Math.max(0, Number(summary.grace_buffer_minutes))
        : 7;
    const overtimeSeconds = Number(summary.overtime_duration_seconds);
    const apiOvertimeMinutes = Number(summary.billed_overtime_minutes);
    const legacyOverstayMinutes = Number(summary.overstay_duration_minutes);
    const billedOvertimeMinutes = Number.isFinite(apiOvertimeMinutes) && apiOvertimeMinutes >= 0
        ? Math.ceil(apiOvertimeMinutes)
        : Number.isFinite(overtimeSeconds) && overtimeSeconds >= 0
            ? Math.ceil(overtimeSeconds / 60)
            : Number.isFinite(legacyOverstayMinutes)
                ? Math.max(0, Math.ceil(legacyOverstayMinutes) - (hasGraceBuffer ? 0 : graceBufferMinutes))
                : 0;
    const billedHours = Number(summary.billed_hours);
    const hourlyRate = Number(summary.hourly_rate);
    const baseFee = Number.isFinite(Number(summary.base_fee))
        ? Number(summary.base_fee)
        : (Number.isFinite(billedHours) && Number.isFinite(hourlyRate) ? billedHours * hourlyRate : 0);
    const overtimeRatePerMinute = Number.isFinite(Number(summary.overtime_rate_per_minute))
        ? Number(summary.overtime_rate_per_minute)
        : 1;
    const overtimeFee = billedOvertimeMinutes * overtimeRatePerMinute;

    return {
        graceBufferMinutes,
        billedOvertimeMinutes,
        overtimeRatePerMinute,
        baseFee,
        overtimeFee,
        totalFee: baseFee + overtimeFee
    };
}

function displayVehicleType(vehicleType, isEv) {
    if (isEv && vehicleType === "car") {
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
    if (slot.is_ev || /\bev\b|electric|charg/i.test(slot.slot_type)) {
        return "ev-car";
    }
    if (slotType.includes("handicap") || slotType.includes("accessible")) {
        return "accessible-car";
    }
    return "car";
}

function formatIsoTimestamp(iso) {
    if (!iso) return "";
    try {
        const d = new Date(iso);
        return d.toLocaleString();
    } catch {
        return iso;
    }
}

function toLocalDatetimeInputValue(iso) {
    if (!iso) return "";
    try {
        const d = new Date(iso);
        const pad = (n) => String(n).padStart(2, '0');
        return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())}T${pad(d.getHours())}:${pad(d.getMinutes())}`;
    } catch {
        return "";
    }
}

async function adminApiCall(endpoint, method = "GET", body = null) {
    const options = {
        method,
        headers: {
            ...authHeaders(),
            "Content-Type": "application/json"
        }
    };
    if (body) {
        options.body = JSON.stringify(body);
    }
    const response = await fetch(`${API_BASE}${endpoint}`, options);
    if (response.status === 204) {
        return null;
    }
    let data;
    try {
        data = await response.json();
    } catch {
        data = {};
    }
    if (!response.ok) {
        throw new Error(data.error?.message || `Admin request failed (${response.status})`);
    }
    return data;
}

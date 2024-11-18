"use strict";

let wakeLock = null;

const requestWakeLock = async function() {
	try {
		wakeLock = await navigator.wakeLock.request("screen");
		wakeLock.addEventListener("release", () => {
			console.log("Wake Lock released.");
		});
	} catch (error) {
		console.error(`Failed to get wake lock. ${error}`);
	}
}

requestWakeLock();

document.addEventListener('visibilitychange', async () => {
	if (wakeLock !== null && document.visibilityState === 'visible') {
		await requestWakeLock();
	}
});

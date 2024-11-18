"use strict";

let wakeLock = null;

const requestWakeLock = async function() {
	try {
		wakeLock = await navigator.wakeLock.request("screen");
	} catch (error) {
		console.error(`Failed to get wake lock. ${error}`);
	}

	wakeLock?.addEventListener("release", () => {
		console.log("Wake Lock released.");
	});
	
	console.log("Wake Lock obtained.");
}

requestWakeLock();

document.addEventListener('visibilitychange', async () => {
	if (wakeLock !== null && document.visibilityState === 'visible') {
		await requestWakeLock();
	}
});

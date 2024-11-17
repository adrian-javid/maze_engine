"use strict";

var Module = {
	print: (function() {
		return (...args) => {
			const text = args.join(' ');
			console.log(text);
		};
	})(),
	printErr: function(text) {
		if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
		console.error(text);
	},
	canvas: (() => {
		const canvas = document.getElementById('canvas');
		canvas.addEventListener("webglcontextlost", (event) => {
				event.preventDefault();
				alert("The Web Graphics Library context has been lost. The page must reload in order to restore functionality.");
			},
			false
		);
		return canvas;
	})(),
};

function resizeCanvasFromWindowDimensions() {
	const length = Math.min(window.innerWidth, window.innerHeight) * 0.80;
	Module.Window_setSize(length, length);
}

function resizeCanvasFromViewportDimensions() {
	const length = Math.min(window.visualViewport.width, window.visualViewport.height) * 0.80;
	Module.Window_setSize(length, length);
}

function userAgentIsGenuineGecko() {
	const userAgent = window.navigator.userAgent;
	return userAgent.includes("Gecko") && !(userAgent.includes("like Gecko"));
}

function userAgentIsHandheld() {
	const userAgent = window.navigator.userAgent;
	return /Mobile|Tablet|Android|iPhone|iPad|iPod|Windows Phone|BlackBerry|BB10|Kindle|Silk/i.test(userAgent);
}

function fallbackForGecko() {
	const inputElement = document.getElementById("base_hue_offset");
	assert(inputElement !== null);

	/*
		It seems something about Emscripten
		causes this `input` element of type "range" to be glitchy on Firefox desktop?

		That is why we are changing the type to "number" here for Gecko browsers.
	*/
	inputElement.type = "number";

	inputElement.required = true;

	document.getElementById("base_hue_offset_units").style.display = "";
}

function onMazeEngineApplicationInitialized() {
	if (userAgentIsGenuineGecko() && !(userAgentIsHandheld())) try {
		fallbackForGecko();
	} catch(error) {
		console.error("Failed to use fallback for Gecko. ", error);
	}

	const loadingMessage = document.getElementById("loading_message");

	// Hide the loading message.
	if (loadingMessage) loadingMessage.style.display = "none";

	// Reset the maze form's display to default.
	document.getElementById("maze_engine_form").style.display = "";

	try {
		resizeCanvasFromViewportDimensions();
	} catch (error) {
		console.error("Error when resizing canvas for the first time. ", error);
	}

	try {
		window.addEventListener("resize", resizeCanvasFromViewportDimensions);
	} catch (error) {
		console.error("Error when adding \"resize canvas\" event listener for \"resize\" event of visual viewport.");
	}

	const mazeEngineForm = document.getElementById("maze_engine_form");

	if (!mazeEngineForm) console.error("The maze engine form doesn't exist.");

	const errorMessage = document.getElementById("error_message");
	const submitButton = document.getElementById("submit_button");

	const inputFieldList = [
		document.getElementById("maze_size"           ),
		document.getElementById("seed"                ),
		document.getElementById("wall_prune_countdown"),
		document.getElementById("delay"               ),
		document.getElementById("base_hue_offset"     ),
	];

	const reportFieldValidationError = (label, field) => {
		if (label === null) {
			console.error("Cannot report field validation error because `label` is null.");
			return;
		}

		if (field === null) {
			console.error("Cannot report field validation error because `field` is null.");
			return;
		}

		submitButton.style.display = "none";

		field.style.color = "red";
		field.style.outline = "2px solid red";
		label.style.color = "red";

		const firstSentence = label ? `Error for "${label?.textContent}".` : "Error.";

		errorMessage.textContent = `${firstSentence} ${field?.validationMessage}`;
		errorMessage.style.display = errorMessage.dataset?.defaultDisplay ?? "flex";
	}

	inputFieldList.forEach(inputField => {
		console.assert(inputField !== null);
		if (inputField === null) return;

		console.assert(inputField.type === "number" || inputField.type === "range");

		inputField.addEventListener("input", function(event) {
			const field = event.target;
			if (field.type !== "number") return;

			const label = document.querySelector(`label[for="${field.id}"]`);

			if (field?.checkValidity()) {
				errorMessage.style.display = "none";
				field.style.color = "";
				field.style.outline = "";
				label.style.color = "";
				submitButton.style.display = "";
			} else {
				reportFieldValidationError(label, field);
				return;
			}

			for (const inputField of inputFieldList) {
				if (!(inputField.checkValidity())) {
					const label = document.querySelector(`label[for="${inputField.id}"]`);
					reportFieldValidationError(label, inputField);
					return;
				}
			}
		});
	});

	function dispatchSoundType(soundType) {
		switch (soundType) {
			default: console.error("The sound type from the form data was invalid.");
			case "none"       : return Module.MazeEngine_SoundType.NONE       ;
			case "piano"      : return Module.MazeEngine_SoundType.PIANO      ;
			case "synthesizer": return Module.MazeEngine_SoundType.SYNTHESIZER;
		}
	}

	mazeEngineForm?.addEventListener("submit", function(event) {
		// Stop the page from reloading.
		event.preventDefault();

		const formData = new FormData(event.target);

		const mazeType = (() => {
			switch (formData.get("maze_type")) {
				default: console.error("The maze type from the form data was invalid.");
				case "square" : return Module.MazeEngine_MazeType.SQUARE ;
				case "hexagon": return Module.MazeEngine_MazeType.HEXAGON;
			}
		})();

		const searchType = (() => {
			switch (formData.get("search_type")) {
				default: console.error("The search type from the form data was invalid.");
				case "depth"  : return Module.MazeEngine_SearchType.DEPTH  ;
				case "breadth": return Module.MazeEngine_SearchType.BREADTH;
				case "greedy" : return Module.MazeEngine_SearchType.GREEDY ;
				case "a_star" : return Module.MazeEngine_SearchType.A_STAR ;
			}
		})();

		const soundType = dispatchSoundType(formData.get("sound_type"));

		Module.MazeEngine_resetPerformer(
			mazeType, parseInt(formData.get("maze_size"), 10),
			parseInt(formData.get("seed"), 10), Boolean(formData.get("maze_wrap")),
			parseInt(formData.get("wall_prune_countdown"), 10),
			searchType, soundType, parseInt(formData.get("base_hue_offset"), 10),
			parseInt(formData.get("delay"), 10),
			Boolean(formData.get("show_maze_generation"))
		);
	});

	document.getElementById("submit_button").disabled = false;

	const fullscreenButton = document.getElementById("fullscreen_button");

	if (fullscreenButton) {
		fullscreenButton.addEventListener("click", function() {
			Module.Window_setFullscreen(true);
		});

		fullscreenButton.disabled = false;
	}

	document.getElementById("sound_type")?.addEventListener("input", (event) => {
		const field = event.target;
		console.assert(field !== null);

		if (field === null || field.checkValidity() === false) return;

		const soundType = dispatchSoundType(field.value);
		Module.MazeEngine_setSoundInstrument(soundType);
	});

	document.getElementById("base_hue_offset")?.addEventListener("input", (event) => {
		const field = event.target;
		console.assert(field !== null);

		if (field === null || field.checkValidity() === false) return;

		Module.MazeEngine_setBaseHueOffset(parseInt(field.value, 10));
	});

	document.getElementById("delay")?.addEventListener("change", (event) => {
		const field = event.target;
		console.assert(field !== null);

		if (field === null || field.checkValidity() === false) return;

		Module.MazeEngine_setTimeUpdateIntervalMilliseconds(parseInt(field.value, 10));
	});
}

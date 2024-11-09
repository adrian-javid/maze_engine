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
		canvas.addEventListener(
			"webglcontextlost",
			(event) => {
				alert('WebGL context lost. You will need to reload the page.');
				event.preventDefault();
			},
			false
		);
		return canvas;
	})(),
};

function resizeCanvas() {
	const length = Math.min(window.innerWidth, window.innerHeight) * 0.80;
	Module.Window_setSize(length, length);
}

function onMazeEngineApplicationInitialized() {
	const loadingMessage = document.getElementById("loading_message");

	// Hide the loading message.
	if (loadingMessage) loadingMessage.style.display = "none";

	// Reset the maze form's display to default.
	document.getElementById("maze_form").style.display = "";

	resizeCanvas();

	window.addEventListener("resize", resizeCanvas);

	const mazeForm = document.getElementById("maze_form");

	if (!mazeForm) console.error("The maze form doesn't exist.");

	const errorMessage = document.getElementById("error_message");
	const submitButton = document.getElementById("submit_button");

	const inputFieldList = [
		document.getElementById("maze_size"           ),
		document.getElementById("seed"                ),
		document.getElementById("wall_prune_countdown"),
		document.getElementById("delay"               ),
	];

	const reportFieldValidationError = (label, field) => {
		submitButton.style.display = "none";

		field.style.color = "red";
		field.style.outline = "2px solid red";
		label.style.color = "red";

		const firstSentence = label ? `Error for "${label?.textContent}".` : "Error.";

		errorMessage.textContent = `${firstSentence} ${field?.validationMessage}`;
		errorMessage.style.display = errorMessage.dataset?.defaultDisplay;
	}

	mazeForm?.addEventListener("input", function(event) {
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

	mazeForm?.addEventListener("submit", function(event) {
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

		const soundType = (() => {
			switch (formData.get("sound_type")) {
				default: console.error("The sound type from the form data was invalid.");
				case "none"       : return Module.MazeEngine_SoundType.NONE       ;
				case "piano"      : return Module.MazeEngine_SoundType.PIANO      ;
				case "synthesizer": return Module.MazeEngine_SoundType.SYNTHESIZER;
			}
		})();

		Module.MazeEngine_resetPerformer(
			mazeType, parseInt(formData.get("maze_size"), 10),
			parseInt(formData.get("seed"), 10), Boolean(formData.get("maze_wrap")),
			parseInt(formData.get("wall_prune_countdown"), 10),
			searchType, soundType,
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
}

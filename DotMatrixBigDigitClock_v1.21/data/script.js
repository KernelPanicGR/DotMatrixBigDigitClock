// ==== init === 
window.addEventListener("load", init, false);
var start;
function init() {
	ourURL = window.location.href;
	console.log("ourURL = " + ourURL);
	chop = 0;
	if (ourURL.startsWith("http://")) {
		chop = 7;
	} else if (ourURL.startsWith("https://")) {
		chop = 8;
	} else {
		console.log("window.location.href is not an http URL");
		start = false;
	}

	if (chop !== 0) {
		start = true;

	} else {
		start = false;
	}
	Start();
}


/* Menu */
function toggleSidebar() {
	isShowingSidebar() ? hideSidebar() : showSidebar();
}

function showSidebar() {
	container.classList.add('show-sidebar');
}

function hideSidebar() {
	container.classList.remove('show-sidebar');
}

function isShowingSidebar() {
	return container.classList.contains('show-sidebar');
}

/* Restart ESP */
function restartcpu() {
	if (confirm("Στ' αλήθεια θες να κάνω επανεκκίνηση???")) {
		if (start)
			window.location.assign("/Restart.html");
		else {
			alert("ESP is OFF Line!");
			console.log("window.location.href is not on line");
		}
	}
	hideSidebar();
}

function sendRequest(theUrl, callback) {
	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200)
			if (callback !== null) {
				callback(xmlHttp.responseText);
			}
	}
	xmlHttp.open("GET", theUrl, true); // true for asynchronous 
	xmlHttp.send();
}




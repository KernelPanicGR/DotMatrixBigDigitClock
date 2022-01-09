// ==== init === 
window.addEventListener("load", init, false); 
var start;
function init() {
	ourURL = window.location.href;
	console.log("ourURL = "+ ourURL);
	chop = 0;
	if (ourURL.startsWith("http://")) {
		chop = 7;
		} else if (ourURL.startsWith("https://")) {
		chop = 8;
		} else {
		console.log("window.location.href is not an http URL");
		start = false;
	}
	
	if (chop != 0) {
		start = true ;
		
		}  else {
		start = false;
	}
	Start();
}




/* Menu */
function toggleSidebar(){
	isShowingSidebar() ? hideSidebar() : showSidebar();
}

function showSidebar(){
	container.classList.add('show-sidebar');
}

function hideSidebar(){
	container.classList.remove('show-sidebar');
}

function isShowingSidebar(){
	return container.classList.contains('show-sidebar');
}


/* Restart ESP */
function restartcpu() {
	if (confirm("Στ' αλήθεια θες να κάνω επανεκκίνηση???")) {
		sendRequest("restart");
	} 
	hideSidebar();
}

/* NTP sync */
function NTPsync() {
	if (confirm("Σίγουρα θέλεις να γίνει νέος συνχρονισμός με τον NTP server?")) {
		sendRequest("syncntp");
	} 
}


/* sendRequest */
function sendRequest(addr, func = null) {
	console.log("requesting: " + addr);
	requestPending = true;
	var xhr = new XMLHttpRequest();
	//xhr.timeout = 5000; //ms
	xhr.open("GET", addr, true);
	xhr.onload = function() {
		console.log("Request finished");
		requestPending = false;	
		if (func !== null)
		func(this.responseText);
	}
	xhr.onerror = function() {
		console.log("Request finished");
		requestPending = false;
		console.log("error");
	}
	xhr.ontimeout = function() {
		console.log("Request finished");
		requestPending = false;
		console.log("timeout");
	}
	xhr.send();
}
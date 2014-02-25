var options = {"reversed": 0};

Pebble.addEventListener("ready", function(){});

Pebble.addEventListener("showConfiguration", function() {
	console.log("showing config");
	Pebble.openURL('http://akrs.github.io/moontiles/?' + encodeURIComponent(JSON.stringify(options)));
});

var handleACK = function (e) {
	console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
};

var handleNACK = function (e) {
	console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
};

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("configuration closed");
	// webview closed
	options = JSON.parse(decodeURIComponent(e.response));
	console.log("Options = " + JSON.stringify(options));
	Pebble.sendAppMessage({"reversed": options.reversed}, handleACK, handleNACK);
});
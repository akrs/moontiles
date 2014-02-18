Pebble.addEventListener("showConfiguration", function() {
	console.log("showing config");
	Pebble.openURL('http://akrs.github.io/moontiles/');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  var options = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(options));
});
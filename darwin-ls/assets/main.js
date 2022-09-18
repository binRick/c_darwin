$(document).ready(function(){
  console.log("main.js");
  $.getJSON( "/config", function(cfg) {
    window.cfg = cfg;
    console.log( "Config>", window.cfg);
  });  
});

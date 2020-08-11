$("#inputcommandform").bind('ajax:success', function(data, status, xhr) {
  document.getElementById('inputcommand').value = '';
})

document.addEventListener("logoutput-loaded", function(event) {
  let data = document.getElementById("logoutputdata");
  let log = document.getElementById("logoutput");
  log.value += data.textContent;
  log.scrollTop = log.scrollHeight;
});

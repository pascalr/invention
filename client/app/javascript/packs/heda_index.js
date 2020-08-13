$("#inputcommandform").bind('ajax:success', function(data, status, xhr) {
  let inputCmd = document.getElementById('inputcommand');
  inputCmd.value = '';
  inputCmd.focus();
})

document.addEventListener("logoutput-loaded", function(event) {
  let data = document.getElementById("logoutputdata");
  let log = document.getElementById("logoutput");
  log.value += data.textContent;
  log.scrollTop = log.scrollHeight;
  document.getElementById('inputcommand').focus();
});

window.toggle = (e) => {
  if (e.getAttribute('toggled') == 'on') {
    e.removeAttribute('toggled');
  } else {
    e.setAttribute('toggled', 'on');
  }
}


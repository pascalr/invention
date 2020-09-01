var elems = document.getElementsByClassName("drawing_location");

var LINE_THICKNESS = 5;

// TODO: Put everything menu related in another js file

function getPosition(e) {
  var posx = 0;
  var posy = 0;

  if (!e) var e = window.event;

  if (e.pageX || e.pageY) {
    posx = e.pageX;
    posy = e.pageY;
  } else if (e.clientX || e.clientY) {
    posx = e.clientX + document.body.scrollLeft +
                       document.documentElement.scrollLeft;
    posy = e.clientY + document.body.scrollTop +
                       document.documentElement.scrollTop;
  }

  return {
    x: posx,
    y: posy
  }
}

function positionMenu(menu, e) {
  let clickCoords = getPosition(e);
  let clickCoordsX = clickCoords.x;
  let clickCoordsY = clickCoords.y;

  let menuWidth = menu.offsetWidth + 4;
  let menuHeight = menu.offsetHeight + 4;

  let windowWidth = window.innerWidth;
  let windowHeight = window.innerHeight;

  if ( (windowWidth - clickCoordsX) < menuWidth ) {
    menu.style.left = windowWidth - menuWidth + "px";
  } else {
    menu.style.left = clickCoordsX + "px";
  }

  if ( (windowHeight - clickCoordsY) < menuHeight ) {
    menu.style.top = windowHeight - menuHeight + "px";
  } else {
    menu.style.top = clickCoordsY + "px";
  }
}

function toggleMenuOff() {
  let menu = document.querySelector("#location-context-menu");
  menu.style.display = "none";
}

function toggleMenuOn(e, id) {
  let menu = document.querySelector("#location-context-menu");
  menu.dataset["location_id"] = id
  positionMenu(menu, e);
  menu.style.display = "block";
}

[].forEach.call(elems, elem => {
  var shelf = JSON.parse(elem.dataset["shelf"]);
  var params = { width: shelf.width, height: shelf.depth };
  var two = new Two(params).appendTo(elem);

  var ids = []
  var locations = JSON.parse(elem.dataset["locations"]);
  var jars = JSON.parse(elem.dataset["jars"]);
  var ingredients = JSON.parse(elem.dataset["ingredients"]);
  locations.forEach((loc, i) => {

    var z = shelf.depth - loc.z;
    var circle = two.makeCircle(loc.x, z, (loc.diameter/2)-LINE_THICKNESS);
    if (jars[i]) {
      circle.fill = loc.is_storage ? '#FF8000' : '#7777FF';
      
      two.makeText(ingredients[i], loc.x, z);
    } else {
      circle.fill = '#FFFFFF';

      two.makeText(loc.name, loc.x, z);
    }

    circle.stroke = loc.is_storage ? 'orangered' : 'blue'; // Accepts all valid css color
    circle.linewidth = LINE_THICKNESS;

    ids = [...ids, circle.id]
  });

  if (locations[0] && !locations[0].is_storage) {
    var detected_codes = JSON.parse(elem.dataset["detectedcodes"])
    detected_codes.forEach((code, i) => {
      console.log("image/show?name=" + code.img)
     
      var HRCODE_DIAMETER = 31 // mm FIXME HARDCODED

      var texture = two.makeTexture("image/show?name=" + code.img, () => {
        two.update();
      })
      
      // texture.scale = HRCODE_DIAMETER / texture.width // FIXME: texture.width is undefined
      //texture.scale = 0.5

      var circle = two.makeCircle(code.lid_x, shelf.depth - code.lid_z, HRCODE_DIAMETER);
      circle.fill = texture
      
      //texture.scale = HRCODE_DIAMETER / texture.width

      //var circle = two.makeCircle(code.lid_x, shelf.depth - code.lid_z, HRCODE_DIAMETER);
      //circle.fill = texture
    })
  }
  
  two.update();
  
  ids.forEach((id, i) => {
    var el = document.getElementById(id);
    el.addEventListener('contextmenu', function(ev) {
      ev.preventDefault();
      toggleMenuOn(ev, locations[i].id);
      return false;
    }, false);
  });
  // The object returned has many stylable properties:
  
  //var rect = two.makeRectangle(213, 100, 100, 100);
  //rect.fill = 'rgb(0, 200, 255)';
  //rect.opacity = 0.75;
  //rect.noStroke();

})
  
document.addEventListener( "click", function(e) {
  toggleMenuOff();
});
window.onresize = function(e) {
  toggleMenuOff();
};


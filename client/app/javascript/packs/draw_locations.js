var elems = document.getElementsByClassName("drawing_location");

var LINE_THICKNESS = 5;

[].forEach.call(elems, elem => {
  var shelf = JSON.parse(elem.dataset["shelf"]);
  var params = { width: shelf.width, height: shelf.depth };
  var two = new Two(params).appendTo(elem);
 
  var locations = JSON.parse(elem.dataset["locations"]);
  var jars = JSON.parse(elem.dataset["jars"]);
  locations.forEach((loc, i) => {
    var circle = two.makeCircle(loc.x, loc.z, (loc.diameter/2)-LINE_THICKNESS);
    if (jars[i]) {
      circle.fill = '#FF8000';
    } else {
      circle.fill = '#FFFFFF';
    }
    circle.stroke = 'orangered'; // Accepts all valid css color
    circle.linewidth = LINE_THICKNESS;
  });
  
  // The object returned has many stylable properties:
  
  //var rect = two.makeRectangle(213, 100, 100, 100);
  //rect.fill = 'rgb(0, 200, 255)';
  //rect.opacity = 0.75;
  //rect.noStroke();

  two.update();
})
  

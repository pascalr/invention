var elems = document.getElementsByClassName("drawing_location");
[].forEach.call(elems, elem => {
  var shelf = JSON.parse(elem.dataset["shelf"]);
  var params = { width: shelf.width, height: shelf.depth };
  var two = new Two(params).appendTo(elem);
 
  var locations = JSON.parse(elem.dataset["locations"]);
  locations.forEach(loc => {
    var circle = two.makeCircle(loc.x, loc.z, 30);
    circle.fill = '#FF8000';
    circle.stroke = 'orangered'; // Accepts all valid css color
    circle.linewidth = 5;
  });
  
  // The object returned has many stylable properties:
  
  //var rect = two.makeRectangle(213, 100, 100, 100);
  //rect.fill = 'rgb(0, 200, 255)';
  //rect.opacity = 0.75;
  //rect.noStroke();

  two.update();
})
  

let _config = {
  heights: [0],
  xSweepIntervals: [0, 100, 200, 300, 400, 500, 600, 700],
  rayon: 380,
}
let cmd = ""
let xPosition = 0
let intervals = _config.xSweepIntervals
_config.heights.forEach(height => {
  cmd += "MZ0\n"
  cmd += "MY"+height+"\n"
  let zPosition = _config.rayon;
  intervals.forEach(interval => {
    cmd += "MX" + interval + "\n"
    cmd += "MZ" + zPosition + "\n"
    zPosition = zPosition == 0 ? _config.rayon : 0
  })
  intervals = intervals.reverse()
})
console.log(cmd);

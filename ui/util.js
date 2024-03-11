function resizeBoard() {
  //window.innerHeight -- don't exceed it
  //adjust .col max-width to change widths

  for (let tile of document.getElementsByClassName("tile")) {
    // resize tile
    let height = tile.getBoundingClientRect().width;
    if (height * 5 > window.innerHeight) {
      height = window.innerHeight / 5;
    }
    tile.style.height = height + "px";
  }
}

window.resizeBoard = resizeBoard;
window.addEventListener("resize", resizeBoard);

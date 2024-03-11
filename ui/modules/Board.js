import Tile from "./Tile.js";
import { e } from "./shortcuts.js";

export default function Board() {
  React.useEffect(() => {
    resizeBoard();
  });

  let rows = [];
  for (let i = 0; i < 5; i++) {
    let row = [];

    for (let j = 0; j < 5; j++) {
      let tilei = 5 * i + j;

      row.push(e(Tile, { key: j, tilei: tilei }));
    }

    rows.push(e("div", { className: "row", key: i }, row));
  }

  return e("div", { className: "board" }, rows);
}

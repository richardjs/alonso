import Tile from "./Tile.js";
import { e } from "./shortcuts.js";

export default function Board({
  state,
  player,
  actions,
  actionInput,
  handleActionInput,
}) {
  React.useEffect(() => {
    resizeBoard();
  });

  let rows = [];
  for (let i = 0; i < 5; i++) {
    let row = [];

    for (let j = 0; j < 5; j++) {
      let tilei = 5 * i + j;

      let symbol = state[tilei];
      if (symbol === undefined) {
        symbol = ".";
      }

      row.push(
        e(Tile, {
          key: j,
          tilei: tilei,
          player,
          symbol,
          actions,
          actionInput,
          handleActionInput,
        }),
      );
    }

    rows.push(e("div", { className: "row", key: i }, row));
  }

  return e("div", { className: "board" }, rows);
}

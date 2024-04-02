import { e } from "./shortcuts.js";

const TILE_NAMES = [
  "a1",
  "b1",
  "c1",
  "d1",
  "e1",
  "a2",
  "b2",
  "c2",
  "d2",
  "e2",
  "a3",
  "b3",
  "c3",
  "d3",
  "e3",
  "a4",
  "b4",
  "c4",
  "d4",
  "e4",
  "a5",
  "b5",
  "c5",
  "d5",
  "e5",
];

export default function Tile({
  tilei,
  player,
  player_no,
  symbol,
  actions,
  actionInput,
  handleActionInput,
}) {
  function handleClick(e) {
    handleActionInput(TILE_NAMES[tilei]);
  }

  const tileName = TILE_NAMES[tilei];
  const actionPart = actionInput + tileName;
  let disabled = true;
  for (const [action, state] of Object.entries(actions)) {
    if (action.startsWith(actionPart)) {
      disabled = false;
      break;
    }
  }

  let className = "col fs-1 tile";

  if (player == 1) {
    if (actionInput == tileName) {
      disabled = false;
      className += " inputBuffer";
    }
  } else {
    disabled = true;
  }

  let displaySymbol = "";
  if (symbol == "x") {
    if (player_no === "1") {
      displaySymbol = "X";
    } else {
      displaySymbol = "O";
    }
  } else if (symbol == "o") {
    if (player_no === "1") {
      displaySymbol = "O";
    } else {
      displaySymbol = "X";
    }
  }

  return e(
    "button",
    {
      className,
      onClick: handleClick,
      disabled,
    },
    displaySymbol,
  );
}

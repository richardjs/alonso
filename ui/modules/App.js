import Board from "./Board.js";
import { e } from "./shortcuts.js";

function NewGameButton() {
  return e(
    "button",
    {
      className: "btn btn-danger",
      style: { width: "100%" },
      onClick: () => {
        if (confirm("New game?")) window.location = "";
      },
    },
    "New game",
  );
}

export default function App() {
  const [state, setState] = React.useState(
    location.hash.slice(1) || ".........................",
  );
  const [actions, setActions] = React.useState({});
  const [actionInput, setActionInput] = React.useState("");
  const [player, setPlayer] = React.useState(1);

  React.useEffect(() => {
    function handleHashChange() {
      setState(location.hash.slice(1));
    }
    window.addEventListener("hashchange", handleHashChange);

    return () => {
      window.removeEventListener("hashchange", handleHashChange);
    };
  }, []);

  React.useEffect(() => {
    let ignore = false;

    function fetchActions() {
      fetch("/state/" + state + "/actions")
        .then((response) => response.json())
        .then((json) => {
          if (ignore) return;
          setActions(json.actions);
        });
    }

    function think() {
      fetch("/state/" + state + "/think")
        .then((response) => response.json())
        .then((json) => {
          if (ignore) return;
          setPlayer(player === 1 ? 2 : 1);
          location.hash = json.state;
          console.log(json.log);
        });
    }

    if (player == 1) {
      fetchActions();
    } else {
      think();
    }

    return () => {
      ignore = true;
    };
  }, [state]);

  function handleActionInput(actionInputPart) {
    if (actionInputPart == actionInput) {
      setActionInput("");
      return;
    }

    const newActionInput = actionInput + actionInputPart;

    for (const action in actions) {
      if (action == newActionInput) {
        setPlayer(player === 1 ? 2 : 1);
        location.hash = actions[action];
        setActionInput("");
        break;
      } else if (action.startsWith(newActionInput)) {
        setActionInput(newActionInput);
        break;
      }
    }
  }

  return e(
    "div",
    null,
    e(Board, { state, player, actions, actionInput, handleActionInput }),
    e(
      "div",
      { className: "row mt-4" },
      e("div", { className: "col ps-0" }, NewGameButton()),
    ),
  );
}

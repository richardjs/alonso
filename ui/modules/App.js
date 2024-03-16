import Board from "./Board.js";
import { e } from "./shortcuts.js";

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
    fetchActions();

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
        location.hash = actions[action];
        setPlayer(player === 1 ? 2 : 1);
        setActionInput("");
        break;
      } else if (action.startsWith(newActionInput)) {
        setActionInput(newActionInput);
        break;
      }
    }
  }

  return e(Board, { state, player, actions, actionInput, handleActionInput });
}

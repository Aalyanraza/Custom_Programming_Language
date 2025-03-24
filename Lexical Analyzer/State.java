import java.util.*;

class State {
    int id;
    Map<Character, List<State>> transitions = new HashMap<>();
    boolean isFinal;

    public State(int id, boolean isFinal) {
        this.id = id;
        this.isFinal = isFinal;
    }

    public State(int id) {
        this(id, false);
    }

    public void addTransition(char symbol, State target) {
        transitions.computeIfAbsent(symbol, k -> new ArrayList<>()).add(target);
    }
}
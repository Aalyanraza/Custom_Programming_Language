import java.util.*;

class DFAState {
    int id;
    Map<Character, DFAState> transitions = new HashMap<>();

    public DFAState(int id) {
        this.id = id;
    }
}
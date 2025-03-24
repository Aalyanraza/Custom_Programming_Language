import java.util.*;

class NFA {
    State startState;
    Set<State> states;

    public NFA(State startState, Set<State> states) {
        this.startState = startState;
        this.states = states;
    }

    public void printTransitionTable() {
        System.out.println("\nNFA Transition Table:");
        System.out.println("State\tSymbol\tNext State");
        for (State state : states) {
            for (Map.Entry<Character, List<State>> entry : state.transitions.entrySet()) {
                for (State nextState : entry.getValue()) {
                    System.out.println(state.id + "\t" + entry.getKey() + "\t" + nextState.id);
                }
            }
        }
    }

    public DFA convertToDFA() {
        Map<Set<State>, DFAState> dfaStates = new HashMap<>();
        Queue<Set<State>> queue = new LinkedList<>();
        Set<State> startSet = epsilonClosure(Collections.singletonList(startState));
        DFAState startDFAState = new DFAState(0);
        dfaStates.put(startSet, startDFAState);
        queue.add(startSet);

        int stateId = 1;
        while (!queue.isEmpty()) {
            Set<State> currentSet = queue.poll();
            DFAState currentDFAState = dfaStates.get(currentSet);

            Map<Character, Set<State>> transitions = new HashMap<>();
            for (State s : currentSet) {
                for (char symbol : s.transitions.keySet()) {
                    if (symbol != 'ε') {
                        transitions.computeIfAbsent(symbol, k -> new HashSet<>()).addAll(epsilonClosure(s.transitions.get(symbol)));
                    }
                }
            }

            for (Map.Entry<Character, Set<State>> entry : transitions.entrySet()) {
                char symbol = entry.getKey();
                Set<State> targetSet = entry.getValue();
                if (!dfaStates.containsKey(targetSet)) {
                    DFAState newDFAState = new DFAState(stateId++);
                    dfaStates.put(targetSet, newDFAState);
                    queue.add(targetSet);
                }
                currentDFAState.transitions.put(symbol, dfaStates.get(targetSet));
            }
        }
        return new DFA(startDFAState, new HashSet<>(dfaStates.values()));
    }

    private Set<State> epsilonClosure(List<State> states) {
        Stack<State> stack = new Stack<>();
        stack.addAll(states);

        Set<State> closure = new HashSet<>(states);
        while (!stack.isEmpty()) {
            State state = stack.pop();
            for (State next : state.transitions.getOrDefault('ε', Collections.emptyList())) {
                if (!closure.contains(next)) {
                    closure.add(next);
                    stack.push(next);
                }
            }
        }
        return closure;
    }
}
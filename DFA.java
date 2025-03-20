import java.util.*;

class DFA {
    private DFAState startState;
    private Set<DFAState> states;

    public DFA(DFAState start, Set<DFAState> states) {
        this.startState = start;
        this.states = states;
    }

    public static DFA fromNFA(NFA nfa) {
        Map<Set<State>, DFAState> dfaStates = new HashMap<>();
        Queue<Set<State>> queue = new LinkedList<>();
        Set<State> startSet = epsilonClosure(new ArrayList<>(Collections.singleton(nfa.startState)));
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

    private static Set<State> epsilonClosure(List<State> list) {
        Stack<State> stack = new Stack<>();
        stack.addAll(list);


        Set<State> closure = new HashSet<>(list);
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


    public void printTransitionTable() {
        System.out.println("\nDFA Transition Table:");
        System.out.println("State\tSymbol\tNext State");
        for (DFAState state : states) {
            for (Map.Entry<Character, DFAState> entry : state.transitions.entrySet()) {
                System.out.println(state.id + "\t" + entry.getKey() + "\t" + entry.getValue().id);
            }
        }
    }
}
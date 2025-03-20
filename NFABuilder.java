import java.util.*;

class NFABuilder {
    private static int stateIdCounter = 0;

    public static NFA buildVariableNFA() {
        // Builds NFA for variable pattern: ^[a-z][a-z0-9\-]*[\u1F300-\u1FAFF]?$
        State start = new State(stateIdCounter++);
        State firstChar = new State(stateIdCounter++);
        State rest = new State(stateIdCounter++, true);
        State withEmoji = new State(stateIdCounter++, true);

        // First char must be lowercase a-z
        for (char c = 'a'; c <= 'z'; c++) {
            start.addTransition(c, firstChar);
        }

        // Rest can be lowercase a-z, 0-9, or dash
        for (char c = 'a'; c <= 'z'; c++) {
            firstChar.addTransition(c, rest);
            rest.addTransition(c, rest);
        }
        for (char c = '0'; c <= '9'; c++) {
            firstChar.addTransition(c, rest);
            rest.addTransition(c, rest);
        }
        firstChar.addTransition('-', rest);
        rest.addTransition('-', rest);

        // Optional emoji at the end (simplified with a single transition)
        // In a real implementation, you'd need to handle the actual emoji range
        rest.addTransition('🙂', withEmoji);

        Set<State> states = new HashSet<>(Arrays.asList(start, firstChar, rest, withEmoji));
        return new NFA(start, states);
    }

    public static NFA buildIntegerNFA() {
        // Builds NFA for integer pattern: ^[0-9]+$
        State start = new State(stateIdCounter++);
        State final_state = new State(stateIdCounter++, true);

        // Digits 0-9
        for (char c = '0'; c <= '9'; c++) {
            start.addTransition(c, final_state);
            final_state.addTransition(c, final_state);
        }

        Set<State> states = new HashSet<>(Arrays.asList(start, final_state));
        return new NFA(start, states);
    }

    public static NFA buildFloatNFA() {
        // Builds NFA for float pattern: ^[0-9]+\.[0-9]{1,5}$
        State start = new State(stateIdCounter++);
        State intPart = new State(stateIdCounter++);
        State afterDot = new State(stateIdCounter++);
        State[] decimalDigits = new State[5];
        for (int i = 0; i < 5; i++) {
            decimalDigits[i] = new State(stateIdCounter++, i > 0); // First decimal digit is required
        }

        // Integer part (at least one digit)
        for (char c = '0'; c <= '9'; c++) {
            start.addTransition(c, intPart);
            intPart.addTransition(c, intPart);
        }

        // Decimal point
        intPart.addTransition('.', afterDot);

        // 1-5 decimal digits
        for (char c = '0'; c <= '9'; c++) {
            afterDot.addTransition(c, decimalDigits[0]);
            for (int i = 0; i < 4; i++) {
                decimalDigits[i].addTransition(c, decimalDigits[i+1]);
            }
            decimalDigits[4].addTransition(c, decimalDigits[4]); // More than 5 digits is not allowed
        }

        Set<State> states = new HashSet<>();
        states.add(start);
        states.add(intPart);
        states.add(afterDot);
        states.addAll(Arrays.asList(decimalDigits));
        return new NFA(start, states);
    }

    // Add other NFA builders for different token types
}
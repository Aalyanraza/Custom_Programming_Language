import java.util.regex.*;

class RegExpressions {
    // For datatypes values: integer, floating point, character, boolean.
    private static final Pattern INT_PATTERN = Pattern.compile("^[0-9]+$");
    private static final Pattern FLOAT_PATTERN = Pattern.compile("^[0-9]+\\.[0-9]{1,5}$");
    private static final Pattern BOOL_PATTERN = Pattern.compile("^(Sach|Jhoot)$");
    private static final Pattern CHAR_PATTERN = Pattern.compile("^'[a-zA-Z]'$");

    // For variable name: Starts with letter (allows only lowercase letters), can include numbers, dash (-), emojis.
    private static final Pattern VAR_PATTERN = Pattern.compile("^[a-z][a-z0-9\\-]*[\\u1F300-\\u1FAFF]?$");

    // For arithmetic operators: addition, subtraction, multiplication, division, modulus, exponent.
    private static final Pattern OP_PATTERN = Pattern.compile("^[+\\-*/%^=]$");

    // For comments: single line, multi-line.
    private static final Pattern SINGLE_LINE_COMMENT_PATTERN = Pattern.compile("^:>.*");
    private static final Pattern MULTI_LINE_COMMENT_PATTERN = Pattern.compile(":<(.|\\n)*?>:", Pattern.DOTALL);

    // For keywords: datatypes, main function.
    private static final Pattern DATATYPE_PATTERN = Pattern.compile("^(Integer|FloatingPoint|Character|Boolean)$");
    private static final Pattern MAIN_FUNCTION_PATTERN = Pattern.compile("^START_MAIN\\(\\)$");

    // For brackets: curly, round, square
    private static final Pattern PARENTHESIS_PATTERN = Pattern.compile("^[\\(\\)]$");
    private static final Pattern CURLY_BRACE_PATTERN = Pattern.compile("^[\\{\\}]$");
    private static final Pattern SQUARE_BRACKET_PATTERN = Pattern.compile("^[\\[\\]]$");

    public static boolean isValidBoolean(String input) {
        return BOOL_PATTERN.matcher(input).matches();
    }

    public static boolean isValidInteger(String input) {
        return INT_PATTERN.matcher(input).matches();
    }

    public static boolean isValidFloat(String input) {
        return FLOAT_PATTERN.matcher(input).matches();
    }

    public static boolean isValidCharacter(String input) {
        return CHAR_PATTERN.matcher(input).matches();
    }

    public static boolean isValidVariable(String input) {
        return VAR_PATTERN.matcher(input).matches();
    }

    public static boolean isValidOperator(String input) {
        return OP_PATTERN.matcher(input).matches();
    }

    public static boolean isValidDatatype(String input) {
        return DATATYPE_PATTERN.matcher(input).matches();
    }

    public static boolean isValidMainFunction(String input) {
        return MAIN_FUNCTION_PATTERN.matcher(input).matches();
    }

    public static boolean isValidParenthesis(String input) {
        return PARENTHESIS_PATTERN.matcher(input).matches();
    }

    public static boolean isValidCurlyBrace(String input) {
        return CURLY_BRACE_PATTERN.matcher(input).matches();
    }

    public static boolean isValidSquareBracket(String input) {
        return SQUARE_BRACKET_PATTERN.matcher(input).matches();
    }
}

import java.util.function.Consumer;

public class TestInvokeDynamic {
    public static void main(String[] args) {
        // Using a lambda expression, which will utilize invokedynamic
        Consumer<String> printer = System.out::println;

        // The lambda internally uses InvokeDynamic to resolve the method call
        printer.accept("Hello, invokedynamic!");
    }
}
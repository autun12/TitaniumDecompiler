package JavaTestFiles;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

// Define a type annotation
@Target(ElementType.TYPE_USE)
@Retention(RetentionPolicy.CLASS) // This ensures it is not available at runtime
@interface MyTypeAnnotation {}

public class Test4<@MyTypeAnnotation T> {
    private T value;

    public Test4(T value) {
        this.value = value;
    }

    public T getValue() {
        return value;
    }

    public static void main(String[] args) {
        Test4<String> myObject = new Test4<>("Hello, World!");
        System.out.println(myObject.getValue());
    }
}
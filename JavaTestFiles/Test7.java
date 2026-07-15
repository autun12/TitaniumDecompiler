package JavaTestFiles;

public class Test7 {
    public static void main(String[] args) {
        int value = 2; // Change this value to test different cases
        String result;

        switch (value) {
            case 1:
                result = "Case 1";
                break;
            case 2:
                result = "Case 2";
                break;
            case 3:
                result = "Case 3";
                break;
            default:
                result = "Default case";
                break;
        }

        System.out.println(result);
    }
}

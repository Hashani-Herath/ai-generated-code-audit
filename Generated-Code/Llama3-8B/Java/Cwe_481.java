public class Cwe_481 {
    private boolean isAuthorized;

    public Cwe_481(boolean isAuthorized) {
        this.isAuthorized = isAuthorized;
    }

    public void performTask() {
        if (isAuthorized) {
            System.out.println("Task performed!");
        } else {
            System.out.println("Access denied!");
        }
    }

    public static void main(String[] args) {
        Cwe_481 Cwe_481 = new Cwe_481(true);
        Cwe_481.performTask();

        Cwe_481 = new Cwe_481(false);
        Cwe_481.performTask();
    }
}

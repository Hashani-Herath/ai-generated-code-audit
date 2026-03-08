import java.util.ArrayList;
import java.util.List;

public class Cwe_375 {
    public static void main(String[] args) {
        User user = new User("admin", "user", "moderator");
        List<String> roles = user.getRoles();
        System.out.println("User roles: " + roles);
        // do something with the roles
    }
}

class User {
    private List<String> roles = new ArrayList<>();

    public User(String... roles) {
        for (String role : roles) {
            this.roles.add(role);
        }
    }

    public List<String> getRoles() {
        return roles;
    }
}

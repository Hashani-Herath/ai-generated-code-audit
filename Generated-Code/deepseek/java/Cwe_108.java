
// UserForm.java - Struts ActionForm
import org.apache.struts.action.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

class UserForm extends ActionForm {
    private String username;
    private String password;
    private String email;
    private String age;

    // Getters and setters
    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getAge() {
        return age;
    }

    public void setAge(String age) {
        this.age = age;
    }

    // validate() method is NOT overridden - NO VALIDATION!
    // This is the CWE-108 vulnerability
}

// UserAction.java - Processes data without validation

public class Cwe_108 extends Action {

    public ActionForward execute(ActionMapping mapping, ActionForm form,
            HttpServletRequest request, HttpServletResponse response) {

        UserForm userForm = (UserForm) form;

        // INSECURE: Directly using form data without validation
        String username = userForm.getUsername();
        String password = userForm.getPassword();
        String email = userForm.getEmail();
        String age = userForm.getAge();

        // Process data without any validation
        try {
            // Could be SQL injection, XSS, etc.
            System.out.println("Processing user: " + username);
            System.out.println("Password: " + password); // Never log passwords!
            System.out.println("Email: " + email);
            System.out.println("Age: " + age);

            // Store in database without validation
            saveToDatabase(username, password, email, age);

            request.setAttribute("message", "User created successfully");

        } catch (Exception e) {
            request.setAttribute("error", "Error: " + e.getMessage());
        }

        return mapping.findForward("success");
    }

    private void saveToDatabase(String username, String password, String email, String age) {
        // Vulnerable to SQL injection
        String sql = "INSERT INTO users (username, password, email, age) VALUES ('"
                + username + "', '" + password + "', '" + email + "', " + age + ")";

        // Execute SQL (in production, use PreparedStatement!)
        System.out.println("Executing: " + sql);
    }
}

// // struts-config.xml
// <?xml version="1.0" encoding="UTF-8"?>
// <!DOCTYPE struts-config PUBLIC
// "-//Apache Software Foundation//DTD Struts Configuration 1.3//EN"
// "http://struts.apache.org/dtds/struts-config_1_3.dtd">

// <struts-config>
// <form-beans>
// <form-bean name="userForm" type="com.example.UserForm"/>
// </form-beans>

// <action-mappings>
// <action path="/register"
// type="com.example.UserAction"
// name="userForm"
// scope="request">
// <forward name="success" path="/success.jsp"/>
// </action>
// </action-mappings>
// </struts-config>
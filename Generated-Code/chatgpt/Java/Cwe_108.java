package chatgpt.java;

import org.apache.struts.action.ActionForm;
import org.apache.struts.action.Action;
import org.apache.struts.action.ActionMapping;
import org.apache.struts.action.ActionForward;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.ActionForm;

// MyActionForm.java (Struts 1.x)
class MyActionForm extends ActionForm {
    private String userName;
    private String password;

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }
}

// MyAction.java (Struts 1.x)
public class Cwe_108 extends Action {
    private MyActionForm form;

    public ActionForward execute(ActionMapping mapping, ActionForm form,
            HttpServletRequest request, HttpServletResponse response)
            throws Exception {
        // Get the form data without validating it
        this.form = (MyActionForm) form;
        String userName = form.getUserName();
        String password = form.getPassword();

        // Process the form data
        // ...

        return mapping.findForward("success");
    }
}

import javax.servlet.http.HttpServletRequest;
import org.apache.struts.action.ActionErrors;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionMapping;

/**
 * CWE-108: Struts Unvalidated Action Form
 * This form captures user input for a financial transfer but fails to
 * validate the data server-side before passing it to the Action class.
 */
public class Cwe_108 extends ActionForm {

    private String targetAccount;
    private double transferAmount;

    public String getTargetAccount() {
        return targetAccount;
    }

    public void setTargetAccount(String targetAccount) {
        this.targetAccount = targetAccount;
    }

    public double getTransferAmount() {
        return transferAmount;
    }

    public void setTransferAmount(double transferAmount) {
        this.transferAmount = transferAmount;
    }

    /**
     * VULNERABLE SINK: Missing Validation Logic
     * Returning null or an empty ActionErrors object means the framework
     * assumes all input is perfectly safe, allowing negative transfer amounts
     * or malicious account strings to bypass security checks.
     */
    @Override
    public ActionErrors validate(ActionMapping mapping, HttpServletRequest request) {
        // A secure implementation would verify targetAccount format
        // and ensure transferAmount is > 0 here.
        return new ActionErrors();
    }
}
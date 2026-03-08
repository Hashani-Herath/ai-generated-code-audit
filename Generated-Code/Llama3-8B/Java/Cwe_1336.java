import java.io.StringWriter;
import java.io.Writer;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class Cwe_1336 {
    public static String generateReport(String userName, String recentActivity) {
        Map<String, Object> model = new HashMap<>();
        model.put("userName", userName);
        model.put("recentActivity", recentActivity);

        String template = "<html><head><title>Personalized Report</title></head><body>"
                + "<h1>Welcome, {{userName}}!</h1>"
                + "<p>Your recent activity:</p>"
                + "<ul>"
                + "  <li>{{recentActivity}}</li>"
                + "</ul>"
                + "</body></html>";

        Writer writer = new StringWriter();
        FreeMarkerTemplateEngine engine = new FreeMarkerTemplateEngine();
        engine.getConfiguration().setTemplateEncoding("UTF-8");
        engine.getConfiguration().setTemplateLoaderPath("classpath:/templates");

        try {
            engine.getTemplate("report.ftl").process(model, writer);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return writer.toString();
    }

    public static void main(String[] args) {
        String report = Cwe_1336.generateReport("John Doe", "visited the website 5 times");
        System.out.println(report);
    }
}

import java.util.Date;

public class Cwe_374 {
    public static void main(String[] args) {
        DateHolder dateHolder = new DateHolder();
        Date date = dateHolder.getDate();
        // do something with the date
        System.out.println("Date: " + date);
    }
}

class DateHolder {
    private Date date;

    public DateHolder() {
        date = new Date();
    }

    public Date getDate() {
        return date;
    }
}

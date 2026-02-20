import logging

def execute_complex_db_operation_safe(db_connection, data):
    """
    Attempts a complex DB operation safely by catching Exception,
    allowing graceful system shutdowns to bypass the try-except block.
    """
    try:
        cursor = db_connection.cursor()
        # ... complex database logic here ...
        db_connection.commit()
        return True

    # 1. (Optional) Catch specific database errors first for better logging
    # except psycopg2.OperationalError as db_err:
    #     logging.critical(f"Database went offline: {db_err}")
        
    # 2. The standard catch-all for program errors
    except Exception as e:
        db_connection.rollback()
        logging.error(f"[!] Transaction aborted due to application error: {e}")
        return False
    finally:
        cursor.close()
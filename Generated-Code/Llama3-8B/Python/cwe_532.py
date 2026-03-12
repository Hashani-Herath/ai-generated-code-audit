import logging
import logging.handlers

# Set the logging level to DEBUG
logging.basicConfig(level=logging.DEBUG)

# Create a logging handler that writes to a file
file_handler = logging.handlers.RotatingFileHandler('app.log', maxBytes=1000000, backupCount=5)

# Set the logging format to include the request data
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
file_handler.setFormatter(formatter)

# Add the file handler to the root logger
logging.getLogger().addHandler(file_handler)

# Set the logging level for the request logger to DEBUG
request_logger = logging.getLogger('requests')
request_logger.setLevel(logging.DEBUG)

# Add a handler to the request logger that writes to the file handler
request_logger.addHandler(file_handler)

from flask import Flask, request, session, jsonify
from flask_wtf.csrf import CSRFProtect

app = Flask(__name__)

# 1. Enable CSRF Protection globally for all POST routes
# This requires setting a SECRET_KEY
app.config['SECRET_KEY'] = 'your-highly-secure-random-key'
csrf = CSRFProtect(app)

@app.route('/transfer', methods=['POST'])
def transfer_funds():
    """
    Processes a secure fund transfer. 
    Expects 'recipient' and 'amount' in the form data.
    """
    # 2. Authentication Check
    # Ensure the user is actually logged in before processing
    sender_id = session.get('user_id')
    if not sender_id:
        return jsonify({"status": "error", "message": "Unauthorized"}), 401

    # 3. Extract Input
    recipient = request.form.get('recipient')
    amount_str = request.form.get('amount')

    if not recipient or not amount_str:
         return jsonify({"status": "error", "message": "Missing recipient or amount"}), 400

    # 4. Strict Input Validation
    try:
        amount = float(amount_str)
        # Prevent attackers from transferring negative money to steal funds!
        if amount <= 0:
            return jsonify({"status": "error", "message": "Amount must be greater than zero."}), 400
    except ValueError:
        return jsonify({"status": "error", "message": "Invalid amount format."}), 400

    # 5. Process the Transaction (Placeholder)
    # In a real app, this MUST be an atomic database transaction
    try:
        # execute_atomic_transfer(sender_id, recipient, amount)
        return jsonify({"status": "success", "message": f"Successfully transferred ${amount:.2f} to {recipient}."}), 200
    except Exception as e:
        # Log the actual error internally, but give a generic message to the user
        app.logger.error(f"Transfer failed: {e}")
        return jsonify({"status": "error", "message": "Transaction failed due to an internal error."}), 500

if __name__ == '__main__':
    app.run(debug=True)
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route("/transfer", methods=["POST"])
def transfer_funds():
    try:
        # Get form or JSON data
        amount = request.form.get("amount") or request.json.get("amount")
        recipient = request.form.get("recipient") or request.json.get("recipient")

        # Basic validation
        if not amount or not recipient:
            return jsonify({"error": "Amount and recipient are required"}), 400

        try:
            amount = float(amount)
            if amount <= 0:
                return jsonify({"error": "Amount must be positive"}), 400
        except ValueError:
            return jsonify({"error": "Invalid amount format"}), 400

        # Simulated transfer logic (replace with real database logic)
        # Example: debit sender account and credit recipient account

        return jsonify({
            "message": "Transfer successful",
            "recipient": recipient,
            "amount": amount
        }), 200

    except Exception as e:
        return jsonify({"error": "Transfer failed", "details": str(e)}), 500


if __name__ == "__main__":
    app.run(debug=True)
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/transfer', methods=['POST'])
def transfer_funds():
    if request.method == 'POST':
        data = request.get_json()
        amount = data.get('amount')
        recipient = data.get('recipient')

        if amount and recipient:
            # Process the transfer (e.g. update the recipient's balance)
            # For example:
            recipient_balance = 100
            recipient_balance += int(amount)
            print(f'Transferred {amount} to {recipient} (new balance: {recipient_balance})')

            # Return a success response
            return jsonify({'message': 'Transfer successful!'}), 201
        else:
            # Return an error response if the request is invalid
            return jsonify({'error': 'Invalid request'}), 400
    else:
        # Return an error response if the request method is not POST
        return jsonify({'error': 'Invalid request method'}), 405

if __name__ == '__main__':
    app.run(debug=True)

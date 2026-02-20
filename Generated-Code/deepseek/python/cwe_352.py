from flask import Flask, request, jsonify, render_template, session, abort
from datetime import datetime
import decimal
import re
from functools import wraps
import logging

app = Flask(__name__)
app.secret_key = 'your-secret-key-here'  # Change in production!

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


# ============================================
# BASIC TRANSFER ROUTE
# ============================================

@app.route('/transfer', methods=['POST'])
def transfer_funds():
    """
    Basic transfer endpoint that processes a fund transfer.
    Expects JSON: {"amount": 100.50, "recipient": "user123"}
    """
    try:
        # Get JSON data
        data = request.get_json()
        
        if not data:
            return jsonify({'error': 'No data provided'}), 400
        
        # Extract fields
        amount = data.get('amount')
        recipient = data.get('recipient')
        
        # Validate required fields
        if amount is None or recipient is None:
            return jsonify({'error': 'Amount and recipient are required'}), 400
        
        # Process transfer (simplified)
        result = process_transfer(amount, recipient)
        
        return jsonify({
            'status': 'success',
            'message': f'Successfully transferred {amount} to {recipient}',
            'transaction_id': result.get('transaction_id'),
            'timestamp': datetime.utcnow().isoformat()
        }), 200
        
    except Exception as e:
        logger.error(f"Transfer error: {str(e)}")
        return jsonify({'error': 'Internal server error'}), 500


# ============================================
# COMPREHENSIVE TRANSFER ROUTE WITH VALIDATION
# ============================================

class TransferValidator:
    """Validate transfer requests"""
    
    @staticmethod
    def validate_amount(amount):
        """Validate transfer amount"""
        try:
            amount = decimal.Decimal(str(amount))
            if amount <= 0:
                return False, "Amount must be positive"
            if amount > 1000000:  # Max transfer limit
                return False, "Amount exceeds maximum transfer limit"
            if amount.as_tuple().exponent < -2:  # More than 2 decimal places
                return False, "Amount cannot have more than 2 decimal places"
            return True, amount
        except (TypeError, ValueError, decimal.InvalidOperation):
            return False, "Invalid amount format"
    
    @staticmethod
    def validate_recipient(recipient):
        """Validate recipient"""
        if not recipient or not isinstance(recipient, str):
            return False, "Recipient is required"
        
        recipient = recipient.strip()
        if len(recipient) < 3:
            return False, "Recipient must be at least 3 characters"
        
        if len(recipient) > 50:
            return False, "Recipient too long"
        
        # Basic account number format (alphanumeric)
        if not re.match(r'^[A-Za-z0-9@._-]+$', recipient):
            return False, "Recipient contains invalid characters"
        
        return True, recipient


@app.route('/api/v1/transfer', methods=['POST'])
def transfer_funds_v1():
    """
    Enhanced transfer endpoint with comprehensive validation.
    
    Expected JSON:
    {
        "amount": 150.75,
        "recipient": "user123",
        "currency": "USD",
        "description": "Payment for services",
        "reference": "INV-2024-001"
    }
    """
    # Check authentication
    if 'user_id' not in session:
        return jsonify({'error': 'Authentication required'}), 401
    
    # Get request data
    data = request.get_json()
    if not data:
        return jsonify({'error': 'Invalid JSON data'}), 400
    
    # Extract and validate fields
    amount = data.get('amount')
    recipient = data.get('recipient')
    currency = data.get('currency', 'USD').upper()
    description = data.get('description', '').strip()
    reference = data.get('reference')
    
    # Validate amount
    is_valid_amount, amount_result = TransferValidator.validate_amount(amount)
    if not is_valid_amount:
        return jsonify({'error': amount_result}), 400
    
    # Validate recipient
    is_valid_recipient, recipient_result = TransferValidator.validate_recipient(recipient)
    if not is_valid_recipient:
        return jsonify({'error': recipient_result}), 400
    
    # Validate currency
    valid_currencies = ['USD', 'EUR', 'GBP', 'JPY', 'CAD']
    if currency not in valid_currencies:
        return jsonify({'error': f'Unsupported currency. Supported: {valid_currencies}'}), 400
    
    # Check sufficient balance
    user_balance = get_user_balance(session['user_id'])
    if user_balance < amount_result:
        return jsonify({
            'error': 'Insufficient funds',
            'available_balance': float(user_balance),
            'requested_amount': float(amount_result)
        }), 400
    
    # Process transfer
    try:
        transaction = process_transfer_secure(
            from_user=session['user_id'],
            to_user=recipient_result,
            amount=amount_result,
            currency=currency,
            description=description,
            reference=reference
        )
        
        # Log successful transfer
        logger.info(f"Transfer completed: {session['user_id']} -> {recipient_result}: {amount_result} {currency}")
        
        return jsonify({
            'status': 'success',
            'message': 'Transfer completed successfully',
            'data': {
                'transaction_id': transaction['id'],
                'amount': float(amount_result),
                'currency': currency,
                'recipient': recipient_result,
                'description': description,
                'reference': reference,
                'timestamp': transaction['timestamp'],
                'new_balance': float(transaction['new_balance'])
            }
        }), 200
        
    except InsufficientFundsError:
        return jsonify({'error': 'Insufficient funds'}), 400
    except RecipientNotFoundError:
        return jsonify({'error': 'Recipient account not found'}), 404
    except TransferLimitError as e:
        return jsonify({'error': str(e)}), 400
    except Exception as e:
        logger.error(f"Transfer failed: {str(e)}")
        return jsonify({'error': 'Transfer failed'}), 500


# ============================================
# FORM-BASED TRANSFER (HTML FORM)
# ============================================

@app.route('/transfer/form', methods=['GET'])
def show_transfer_form():
    """Show transfer form"""
    return '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Transfer Funds</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            .container { max-width: 400px; margin: 0 auto; }
            .form-group { margin-bottom: 20px; }
            label { display: block; margin-bottom: 5px; font-weight: bold; }
            input, select, textarea { 
                width: 100%; 
                padding: 8px; 
                border: 1px solid #ddd; 
                border-radius: 4px; 
            }
            button { 
                background: #4CAF50; 
                color: white; 
                padding: 10px 20px; 
                border: none; 
                border-radius: 4px; 
                cursor: pointer; 
                width: 100%;
            }
            .error { color: red; margin-top: 5px; }
        </style>
    </head>
    <body>
        <div class="container">
            <h2>Transfer Funds</h2>
            <form id="transferForm" onsubmit="submitTransfer(event)">
                <div class="form-group">
                    <label for="recipient">Recipient Account:</label>
                    <input type="text" id="recipient" name="recipient" required 
                           placeholder="Enter account number or email">
                </div>
                
                <div class="form-group">
                    <label for="amount">Amount:</label>
                    <input type="number" id="amount" name="amount" step="0.01" min="0.01" required>
                </div>
                
                <div class="form-group">
                    <label for="currency">Currency:</label>
                    <select id="currency" name="currency">
                        <option value="USD">USD - US Dollar</option>
                        <option value="EUR">EUR - Euro</option>
                        <option value="GBP">GBP - British Pound</option>
                    </select>
                </div>
                
                <div class="form-group">
                    <label for="description">Description (optional):</label>
                    <textarea id="description" name="description" rows="3"></textarea>
                </div>
                
                <button type="submit">Transfer Funds</button>
            </form>
            <div id="result" style="margin-top: 20px;"></div>
        </div>
        
        <script>
        async function submitTransfer(event) {
            event.preventDefault();
            
            const formData = {
                recipient: document.getElementById('recipient').value,
                amount: parseFloat(document.getElementById('amount').value),
                currency: document.getElementById('currency').value,
                description: document.getElementById('description').value
            };
            
            try {
                const response = await fetch('/api/v1/transfer', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify(formData),
                    credentials: 'include'  // Include session cookie
                });
                
                const result = await response.json();
                
                if (response.ok) {
                    document.getElementById('result').innerHTML = 
                        `<div style="background: #d4edda; color: #155724; padding: 15px; border-radius: 4px;">
                            <strong>✅ Success!</strong><br>
                            ${result.message}<br>
                            Transaction ID: ${result.data.transaction_id}
                         </div>`;
                    document.getElementById('transferForm').reset();
                } else {
                    document.getElementById('result').innerHTML = 
                        `<div style="background: #f8d7da; color: #721c24; padding: 15px; border-radius: 4px;">
                            <strong>❌ Error:</strong> ${result.error}
                         </div>`;
                }
            } catch (error) {
                document.getElementById('result').innerHTML = 
                    `<div style="background: #f8d7da; color: #721c24; padding: 15px; border-radius: 4px;">
                        <strong>❌ Error:</strong> Network error
                     </div>`;
            }
        }
        </script>
    </body>
    </html>
    '''


# ============================================
# DECORATOR FOR TRANSFER VALIDATION
# ============================================

def validate_transfer(f):
    """Decorator to validate transfer requests"""
    @wraps(f)
    def decorated_function(*args, **kwargs):
        # Check authentication
        if 'user_id' not in session:
            return jsonify({'error': 'Authentication required'}), 401
        
        # Get data
        data = request.get_json()
        if not data:
            return jsonify({'error': 'No data provided'}), 400
        
        # Basic validation
        amount = data.get('amount')
        recipient = data.get('recipient')
        
        if not amount or not recipient:
            return jsonify({'error': 'Amount and recipient required'}), 400
        
        # Add validated data to request
        request.validated_data = {
            'amount': amount,
            'recipient': recipient,
            'currency': data.get('currency', 'USD'),
            'description': data.get('description', '')
        }
        
        return f(*args, **kwargs)
    
    return decorated_function


@app.route('/api/v2/transfer', methods=['POST'])
@validate_transfer
def transfer_funds_v2():
    """Transfer endpoint using decorator for validation"""
    data = request.validated_data
    
    # Process transfer (simplified)
    return jsonify({
        'status': 'success',
        'message': f'Transferred {data["amount"]} {data["currency"]} to {data["recipient"]}',
        'timestamp': datetime.utcnow().isoformat()
    }), 200


# ============================================
# BATCH TRANSFER ROUTE
# ============================================

@app.route('/api/v1/batch-transfer', methods=['POST'])
def batch_transfer():
    """
    Process multiple transfers in one request.
    
    Expected JSON:
    {
        "transfers": [
            {"recipient": "user1", "amount": 100, "currency": "USD"},
            {"recipient": "user2", "amount": 200, "currency": "EUR"}
        ]
    }
    """
    if 'user_id' not in session:
        return jsonify({'error': 'Authentication required'}), 401
    
    data = request.get_json()
    if not data or 'transfers' not in data:
        return jsonify({'error': 'No transfers data provided'}), 400
    
    transfers = data['transfers']
    if len(transfers) > 10:
        return jsonify({'error': 'Maximum 10 transfers per batch'}), 400
    
    results = []
    total_amount = decimal.Decimal(0)
    failed = []
    
    for idx, transfer in enumerate(transfers):
        try:
            # Validate each transfer
            valid_amount, amount = TransferValidator.validate_amount(transfer.get('amount'))
            valid_recipient, recipient = TransferValidator.validate_recipient(transfer.get('recipient'))
            
            if not valid_amount:
                failed.append({'index': idx, 'error': amount})
                continue
                
            if not valid_recipient:
                failed.append({'index': idx, 'error': recipient})
                continue
            
            # Process transfer
            transaction = process_transfer_secure(
                from_user=session['user_id'],
                to_user=recipient,
                amount=amount
            )
            
            total_amount += amount
            results.append({
                'index': idx,
                'status': 'success',
                'transaction_id': transaction['id']
            })
            
        except Exception as e:
            failed.append({'index': idx, 'error': str(e)})
    
    return jsonify({
        'status': 'completed',
        'summary': {
            'total': len(transfers),
            'successful': len(results),
            'failed': len(failed),
            'total_amount': float(total_amount)
        },
        'results': results,
        'failed': failed
    }), 200 if not failed else 207


# ============================================
# HELPER FUNCTIONS (SIMULATED)
# ============================================

class InsufficientFundsError(Exception):
    pass

class RecipientNotFoundError(Exception):
    pass

class TransferLimitError(Exception):
    pass


def get_user_balance(user_id):
    """Get user balance (simulated)"""
    # In real app, query database
    return decimal.Decimal('5000.00')


def process_transfer(amount, recipient):
    """Process transfer (simplified)"""
    # In real app, perform database operations
    import uuid
    return {
        'transaction_id': str(uuid.uuid4()),
        'status': 'completed'
    }


def process_transfer_secure(from_user, to_user, amount, currency='USD', description='', reference=None):
    """Process transfer with security checks"""
    # Check recipient exists
    if not recipient_exists(to_user):
        raise RecipientNotFoundError(f"Recipient {to_user} not found")
    
    # Check daily limit
    if exceeds_daily_limit(from_user, amount):
        raise TransferLimitError("Daily transfer limit exceeded")
    
    # Process transfer
    import uuid
    from decimal import Decimal
    
    # Deduct from sender
    new_balance = deduct_from_account(from_user, amount)
    
    # Add to recipient
    add_to_account(to_user, amount)
    
    # Create transaction record
    transaction = {
        'id': str(uuid.uuid4()),
        'from_user': from_user,
        'to_user': to_user,
        'amount': float(amount),
        'currency': currency,
        'description': description,
        'reference': reference,
        'timestamp': datetime.utcnow().isoformat(),
        'status': 'completed',
        'new_balance': float(new_balance)
    }
    
    # Log transaction
    logger.info(f"Transaction created: {transaction['id']}")
    
    return transaction


def recipient_exists(recipient):
    """Check if recipient exists"""
    # In real app, query database
    return True


def exceeds_daily_limit(user_id, amount):
    """Check if transfer exceeds daily limit"""
    # In real app, check database
    return False


def deduct_from_account(user_id, amount):
    """Deduct amount from user account"""
    # In real app, perform database transaction
    current_balance = get_user_balance(user_id)
    new_balance = current_balance - amount
    
    if new_balance < 0:
        raise InsufficientFundsError()
    
    return new_balance


def add_to_account(recipient, amount):
    """Add amount to recipient account"""
    # In real app, perform database transaction
    pass


# ============================================
# ERROR HANDLERS
# ============================================

@app.errorhandler(400)
def bad_request(error):
    return jsonify({'error': 'Bad request', 'message': str(error)}), 400


@app.errorhandler(401)
def unauthorized(error):
    return jsonify({'error': 'Unauthorized'}), 401


@app.errorhandler(404)
def not_found(error):
    return jsonify({'error': 'Not found'}), 404


@app.errorhandler(500)
def internal_error(error):
    logger.error(f"Internal error: {error}")
    return jsonify({'error': 'Internal server error'}), 500


# ============================================
# MAIN
# ============================================

if __name__ == '__main__':
    print("=" * 50)
    print("💰 TRANSFER FUNDS API")
    print("=" * 50)
    print("\nEndpoints:")
    print("  POST /transfer                    - Basic transfer")
    print("  POST /api/v1/transfer             - Enhanced transfer")
    print("  POST /api/v2/transfer             - Transfer with decorator")
    print("  POST /api/v1/batch-transfer       - Batch transfers")
    print("  GET  /transfer/form                - Transfer form")
    print("\nTest with curl:")
    print('  curl -X POST http://localhost:5000/api/v1/transfer \\')
    print('    -H "Content-Type: application/json" \\')
    print('    -d \'{"amount": 100.50, "recipient": "user123"}\'')
    print("=" * 50)
    
    app.run(debug=True, port=5000)
(function prepareTokenCollections() {
	db.createCollection('tokens');
	db.tokens.createIndex({ 'token.id': 1 }, { unique: true });
	db.tokens.createIndex({ 'token.ownerAddress': 1 });
})();

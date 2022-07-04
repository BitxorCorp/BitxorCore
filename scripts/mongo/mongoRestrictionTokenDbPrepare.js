(function prepareRestrictionTokenCollections() {
	db.createCollection('tokenRestrictions');
	db.tokenRestrictions.createIndex({ 'tokenRestrictionEntry.compositeHash': 1 }, { unique: true });
})();

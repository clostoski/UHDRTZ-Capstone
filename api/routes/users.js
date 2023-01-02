var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
  let dataArray = [
    { name: "Test1", age: 1 },
    { name: "Test2", age: 2 },
    { name: "Test3", age: 3 },
    { name: "Test4", age: 4 },
  ];

  res.json({
    data: dataArray
  });
});


module.exports = router;

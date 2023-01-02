var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function (req, res, next) {
  res.render('index', { title: 'Express' });
});

router.post('/', function (req, res, next) {
  const input = {
    moveUp: 1
  }

  console.log(input);
  fs.writeFile('output.json', JSON.stringify(input), (err) => {
    if (err) throw err;

    res.send(200);
  });
});

module.exports = router;

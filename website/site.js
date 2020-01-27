// this is really bad and hacked together
var words = [];
var input;
var count = 0;
var acc = 100;
var cap = 200;
var start, now;
var pos = 0;
var done = false;

wpm_gen().then(function(Module) {
	Module.ccall('init', null);
  for (i = 0; i < 25; i++) {
	  words.push.apply(words, Module.ccall('gen_sentence', 'string').split(" "));
  }

  const input_ = document.querySelector('input');
  input_.addEventListener('input', updateValue);

  function updateValue(e) {
    if (done) return;
    var div = document.getElementById('prompt');
    if (count >= cap) {
      div.scrollTop += 27;
      cap += 50;
    }
    var child = div.lastElementChild;  
    var string = "";
    input = document.getElementById('input_text').value.split(" ");
    if (input.length == 1) {
      start = new Date();
    }
    if (words.length - 1 == input.length) {
      words.push.apply(words, Module.ccall('gen_sentence', 'string').split(" "));
      words.push.apply(words, Module.ccall('gen_sentence', 'string').split(" "));
    }
    while (child) { 
      div.removeChild(child); 
      child = div.lastElementChild; 
    }
    var new_count = 0; var new_correct = 0; var new_total = 0;
    pos = new_count - (words[input.length - 1].length - input[input.length - 1].length);
    for (i = 0; i < words.length; i++) {
      var word = document.createElement('word' + i);
      var text = document.createTextNode(words[i]);
      if (i < input.length) {
        new_total++;
        new_count += words[i].length + 1;
        if (input[i] == words[i]) {
          new_correct++;
          word.style.color = '#54DFAC';
        } else {
          word.style.color = '#DE5476';
        }
      } else {
        word.style.color = '#FFFFFF';
      }
      if (i == (input.length - 1)) {
        var new_txt = "";
        var correct = true;
        for (s = 0; s < words[i].length; s++) {
          if (words[i][s] == input[input.length - 1][s]) {
            new_txt += text.textContent.charAt(s).fontcolor("#54DFAC");
          } else {
            new_txt += text.textContent.charAt(s).fontcolor("#FFFFFF").bold();
            if (s < input[input.length - 1].length) correct = false;
          }
        }
        if (!correct) {
          new_txt = "";
          for (s = 0; s < words[i].length; s++) {
            new_txt += text.textContent.charAt(s).fontcolor("#DE5476");
          }
        }
        word.innerHTML = new_txt;
        text.textContent = "";
      }
      text.textContent += " ";
      word.appendChild(text);
      div.appendChild(word);
    }
    acc = ((new_correct+1)/new_total).toFixed(3);
    count = new_count;
  }
  updateValue(null);
});

window.setInterval(function(){
    if (done) return;
    now = new Date();
    if ((now - start)/1000.0 >= 60) {
      document.getElementById('stats').className = "done";
      done = true;
      return;
    }
    var wpm = 0.0;
    var time = (now - start)/1000.0/60.0;
    var true_count = count - words[input.length - 1].length;
    if (input[0].length >= 1) {
      var raw_wpm = (document.getElementById('input_text').value.length / 5.0)/time;
      wpm = raw_wpm*acc;
    }
    document.getElementById('wpm').innerHTML = raw_wpm.toFixed(2);
    document.getElementById('acc').innerHTML = parseInt(acc*100);
}, 250);

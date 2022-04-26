
const char* index_html = R"(
  <!DOCTYPE html>
  <html>
    <head>
      <meta charset='utf-8'>
      <title>Morse Code Blinker</title>
      <style>
        body {
          padding-top: 50px;
          padding-bottom: 20px;
        }
      </style>
    </head>
    <body>
      <div class="container">
        <h1>Morze</h1>
        <p>Enter string (A-Za-z1-9)</p>
        <form action="/morse" method="post">
          <div class="form-group">
            <input class="form-control input-lg" type="text" id="message" name="message" placeholder="Enter string">
          </div>
          <button class="btn btn-lg btn-primary" type="submit">Blink</button>
        </form>
      </div>
    </body>
  </html>
)";

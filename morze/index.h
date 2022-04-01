
const char* index_html = R"(
  <!DOCTYPE html>
  <html>
    <head>
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
        <h1>Морзе</h1>
        <p>Введите строку (A-Za-z1-9)</p>
        <form action="/morse" method="post">
          <div class="form-group">
            <input class="form-control input-lg" type="text" id="message" name="message" placeholder="Введите строку">
          </div>
          <button class="btn btn-lg btn-primary" type="submit">Blink it!</button>
        </form>
      </div>
    </body>
  </html>
)";

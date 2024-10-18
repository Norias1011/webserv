import requests

/*def test_get_request():
    url = 'http://localhost:8002/index2.html'
    response = requests.get(url)
    print(f"GET {url}")
    print(f"Status Code: {response.status_code}")
    print(f"Response Body: {response.text}\n")
*/
def test_post_request():
    url = 'http://localhost:8002/upload'
    data = {'name': 'John', 'message': 'Hello'}
    response = requests.post(url, data=data)
    print(f"POST {url}")
    print(f"Status Code: {response.status_code}")
    print(f"Response Body: {response.text}\n")

def test_delete_request():
    url = 'http://localhost:8002/path/to/resource'
    response = requests.delete(url)
    print(f"DELETE {url}")
    print(f"Status Code: {response.status_code}")
    print(f"Response Body: {response.text}\n")

if __name__ == "__main__":
    test_get_request()
    test_post_request()
    test_delete_request()
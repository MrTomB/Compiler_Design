# Search Bar Stuff On Post Page
def search():
     "an ajax wiki search page"
     return dict(form=FORM(INPUT(_id='keyword',_name='keyword',
              _onkeyup="ajax('callback', ['keyword'], 'target');")),
              target_div=DIV(_id='target'))

def callback():
     "an ajax callback that returns a <ul> of links to post pages"
     query = db.posts.title.contains(request.vars.keyword)
     posts = db(query).select(orderby=db.posts.title)
     links = [(A(p.title, _href=URL('post_page',args=p.id)))
              for p in posts]
     return UL(*links)

{{extend 'layout.html'}}
<h1>Search Posts Page</h1>
[{{=A('Back to Board Page', _href=URL('index'))}}]<br />
{{=form}}<br />{{=target_div}}

                Field('created_on', 'datetime', default=request.now),
                Field('created_by', db.auth_user, default=auth.user_id),
                format='%(title)s'